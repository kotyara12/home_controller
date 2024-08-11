#include "ioexp.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "rLog.h"
#include "reEsp32.h"
#include "reMCP23017.h"

static const char* logTAG = "IOEXP";

static const char* ioexpTaskName = "ioexp";
static TaskHandle_t _ioexpTask = nullptr;
static QueueHandle_t _ioexpQueue = nullptr;

#define IOEXP_RESET_TIMEOUT   100
#define IOEXP_START_TIMEOUT   250

static reMCP23017 ioexpOutputs(CONFIG_IOEXP_OUTPUTS_BUS, CONFIG_IOEXP_OUTPUTS_ADDRESS, nullptr);

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------ Инициализация и сброс ------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

bool ioexpInit()
{
  rlog_i(logTAG, "Initialize IO expanders");

  // Настраиваем GPIO аппаратного сброса расширителей
  RE_OK_CHECK(gpio_reset_pin((gpio_num_t)CONFIG_GPIO_IOEXP_RESET), return false);
  RE_OK_CHECK(gpio_set_direction((gpio_num_t)CONFIG_GPIO_IOEXP_RESET, GPIO_MODE_OUTPUT), return false);
  RE_OK_CHECK(gpio_set_pull_mode((gpio_num_t)CONFIG_GPIO_IOEXP_RESET, GPIO_PULLDOWN_ONLY), return false);
  RE_OK_CHECK(gpio_set_level((gpio_num_t)CONFIG_GPIO_IOEXP_RESET, CONFIG_GPIO_LEVEL_ESP_ENABLED), return false);

  // Настраиваем входы прерываний
  RE_OK_CHECK(gpio_reset_pin((gpio_num_t)CONFIG_GPIO_IOEXP_ISR1), return false);
  RE_OK_CHECK(gpio_set_direction((gpio_num_t)CONFIG_GPIO_IOEXP_ISR1, GPIO_MODE_INPUT), return false);
  RE_OK_CHECK(gpio_set_pull_mode((gpio_num_t)CONFIG_GPIO_IOEXP_ISR1, GPIO_PULLUP_ONLY), return false);

  RE_OK_CHECK(gpio_reset_pin((gpio_num_t)CONFIG_GPIO_IOEXP_ISR2), return false);
  RE_OK_CHECK(gpio_set_direction((gpio_num_t)CONFIG_GPIO_IOEXP_ISR2, GPIO_MODE_INPUT), return false);
  RE_OK_CHECK(gpio_set_pull_mode((gpio_num_t)CONFIG_GPIO_IOEXP_ISR2, GPIO_PULLUP_ONLY), return false);

  vTaskDelay(pdMS_TO_TICKS(IOEXP_START_TIMEOUT));

  return true;
}

bool ioexpReset()
{
  rlog_i(logTAG, "Reset IO expanders");

  // Аппаратный сброс расширителей
  RE_OK_CHECK(gpio_set_level((gpio_num_t)CONFIG_GPIO_IOEXP_RESET, CONFIG_GPIO_LEVEL_ESP_DISABLED), return false);
  vTaskDelay(pdMS_TO_TICKS(IOEXP_RESET_TIMEOUT));
  RE_OK_CHECK(gpio_set_level((gpio_num_t)CONFIG_GPIO_IOEXP_RESET, CONFIG_GPIO_LEVEL_ESP_ENABLED), return false);
  vTaskDelay(pdMS_TO_TICKS(IOEXP_START_TIMEOUT));
  
  // Настраиваем микросхему на основной плате
  ioexpOutputs.configSet(MCP23017_OPEN_DRAIN, true);
  ioexpOutputs.portSetMode(0x0000);
  ioexpOutputs.portWrite(0x0000);

  return true;
}

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------- Внешнее управление --------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

bool ioexpSetLevel(uint8_t pin, bool level)
{
  ioexp_command_t cmd;
  cmd.type = level ? IOEXP_LVL_1 : IOEXP_LVL_0;
  cmd.pin = pin;
  return xQueueSend(_ioexpQueue, &cmd, pdMS_TO_TICKS(CONFIG_IOEXP_QUEUE_TIMEOUT)) == pdTRUE;
}

bool ioexpResetTimer(uint8_t pin)
{
  ioexp_command_t cmd = {IOEXP_TIMER, pin};
  return xQueueSend(_ioexpQueue, &cmd, pdMS_TO_TICKS(CONFIG_IOEXP_QUEUE_TIMEOUT)) == pdTRUE;
}

bool ioexpResetMcp()
{
  ioexp_command_t cmd = {IOEXP_RESET, 0};
  return xQueueSend(_ioexpQueue, &cmd, pdMS_TO_TICKS(CONFIG_IOEXP_QUEUE_TIMEOUT)) == pdTRUE;
}

// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------- Таймер сброса ----------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

static esp_timer_handle_t reset_timer = nullptr;

void ioexpSmokeResetTimerFree()
{
  if (reset_timer) {
    if (esp_timer_is_active(reset_timer)) esp_timer_stop(reset_timer);
    esp_timer_delete(reset_timer);
    reset_timer = nullptr;
  };
}

void ioexpSmokeResetTimerCb(void* arg)
{
  ioexpSmokeResetTimerFree();
  ioexp_command_t cmd = {IOEXP_LVL_0, *(uint8_t*)arg};
  xQueueSend(_ioexpQueue, &cmd, portMAX_DELAY);
}

bool ioexpSmokeReset(uint8_t pin)
{
  ioexpOutputs.pinWrite(pin, 1);
  
  esp_timer_create_args_t tmr_cfg = {
    .callback = ioexpSmokeResetTimerCb,
    .arg = &pin,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "reset_timer",
    .skip_unhandled_events = false,
  };
  if ((esp_timer_create(&tmr_cfg, &reset_timer) != ESP_OK) 
    || (reset_timer == nullptr) 
    || (esp_timer_start_once(reset_timer, CONFIG_RESET_SMOKE_TIMEOUT_S * 1000000) != ESP_OK)) 
  {
    ioexpSmokeResetTimerFree();
    vTaskDelay(pdMS_TO_TICKS(CONFIG_RESET_SMOKE_TIMEOUT_S * 1000));
    ioexpOutputs.pinWrite(pin, 0);
    return false;
  };
  return true;
}

// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------- Задача -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

void ioexpTaskExec(void *pvParameters)
{
  ioexpReset();
  
  static ioexp_command_t cmd;
  while (1) {
    if (xQueueReceive(_ioexpQueue, &cmd, portMAX_DELAY) == pdTRUE) {
      // Управление уровнями на выходных GPIO
      if (cmd.type == IOEXP_LVL_0) {
        // ioexpOutputs.pinWrite(cmd.pin, 0);
        ioexpOutputs.pinSetLatch(cmd.pin, 0);
      } else if (cmd.type == IOEXP_LVL_1) {
        // ioexpOutputs.pinWrite(cmd.pin, 1);
        ioexpOutputs.pinSetLatch(cmd.pin, 1);
      } 
      // Сброс шлейфа датчиков дыма по таймеру
      else if (cmd.type == IOEXP_RESET) {
        rlog_d(logTAG, "Recieved timer on-off on pin %d", cmd.pin);
        ioexpSmokeReset(cmd.pin);
      }
      // Аппаратный сброс расширителей
      else if (cmd.type == IOEXP_RESET) {
        rlog_d(logTAG, "Recieved expanders reset", cmd.pin);
        ioexpReset();
      };
    };
  };

  vTaskDelete(nullptr);
  espRestart(RR_ERROR);
}

bool ioexpTaskStart()
{
  #if CONFIG_IOEXP_STATIC_ALLOCATION
    static StaticTask_t ioexpTaskBuffer;
    static StaticQueue_t ioexpQueueBuffer;
    static StackType_t ioexpTaskStack[CONFIG_IOEXP_TASK_STACK_SIZE];
    static uint8_t ioexpQueueStorage[CONFIG_IOEXP_QUEUE_SIZE * sizeof(ioexp_command_t)];
    _ioexpQueue = xQueueCreateStatic(CONFIG_IOEXP_QUEUE_SIZE, sizeof(ioexp_command_t), &ioexpQueueStorage[0], &ioexpQueueBuffer);
    _ioexpTask = xTaskCreateStaticPinnedToCore(ioexpTaskExec, ioexpTaskName, 
      CONFIG_IOEXP_TASK_STACK_SIZE, NULL, CONFIG_IOEXP_TASK_PRIORITY, ioexpTaskStack, &ioexpTaskBuffer, CONFIG_IOEXP_TASK_CORE);
  #else
    _ioexpQueue = xQueueCreate(CONFIG_IOEXP_QUEUE_SIZE, sizeof(ioexp_command_t));
    xTaskCreatePinnedToCore(ioexpTaskExec, ioexpTaskName, 
      CONFIG_IOEXP_TASK_STACK_SIZE, NULL, CONFIG_IOEXP_TASK_PRIORITY, &_ioexpTask, CONFIG_IOEXP_TASK_CORE);
  #endif // CONFIG_IOEXP_STATIC_ALLOCATION
  if (_ioexpQueue && _ioexpTask) {
    rloga_i("Task [ %s ] has been successfully created and started", ioexpTaskName);
    return true;
  }
  else {
    rloga_e("Failed to create a task for processing sensor readings!");
    return false;
  };
}

