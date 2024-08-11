#include "sensors.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "mbcontroller.h"
#include "rLog.h"
#include "rStrings.h"
#include "reEsp32.h"
#include "reMqtt.h"
#include "reBeep.h"
#include "reLed.h"
#include "reGpio.h"
#include "reLoadCtrl.h"
#include "ioexp.h"

static const char* logTAG = "SNS";

static const char* sensorsTaskName = "sensors";
static TaskHandle_t _sensorsTask = nullptr;
static EventGroupHandle_t _sensorsFlags = nullptr;

// Служебные биты
#define FLG_STORE_DATA            BIT0
#define FLG_PARAMS_CHANGED        BIT1
#define FLG_TIME_CHANGED          BIT2

// События на входах
#define FLG_INPUT_DOORBTN         BIT3
#define FLG_INPUT_GAS             BIT4

#define FLGS_WAIT                 (FLG_TIME_CHANGED | FLG_INPUT_DOORBTN | FLG_INPUT_GAS)
#define FLGS_CLEAR                0x00FFFFFF

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------ Параметры ------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

static paramsGroupHandle_t pgSensors;
static paramsGroupHandle_t pgIntervals;

void sensorsInitParameters()
{
  // Группы
  pgSensors = paramsRegisterGroup(nullptr, 
    CONFIG_SENSOR_PGROUP_ROOT_KEY, CONFIG_SENSOR_PGROUP_ROOT_TOPIC, CONFIG_SENSOR_PGROUP_ROOT_FRIENDLY);
  pgIntervals = paramsRegisterGroup(pgSensors, 
    CONFIG_SENSOR_PGROUP_INTERVALS_KEY, CONFIG_SENSOR_PGROUP_INTERVALS_TOPIC, CONFIG_SENSOR_PGROUP_INTERVALS_FRIENDLY);

  if (pgIntervals) {
    // Период чтения данных с сенсоров
    paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_U32, nullptr, pgIntervals,
      CONFIG_SENSOR_PARAM_INTERVAL_READ_KEY, CONFIG_SENSOR_PARAM_INTERVAL_READ_FRIENDLY,
      CONFIG_MQTT_PARAMS_QOS, (void*)&iSensorsReadInterval);

    // Период публикации данных с сенсоров на MQTT
    paramsRegisterValue(OPT_KIND_PARAMETER, OPT_TYPE_U32, nullptr, pgIntervals,
      CONFIG_SENSOR_PARAM_INTERVAL_MQTT_KEY, CONFIG_SENSOR_PARAM_INTERVAL_MQTT_FRIENDLY,
      CONFIG_MQTT_PARAMS_QOS, (void*)&iMqttPubInterval);
  };
}

// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------- Сенсоры ------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

bool sensorsPublish(rSensor *sensor, char* topic, char* payload, const bool free_topic, const bool free_payload)
{
  return mqttPublish(topic, payload, CONFIG_MQTT_SENSORS_QOS, CONFIG_MQTT_SENSORS_RETAINED, free_topic, free_payload);
}

void sensorsMqttTopicsCreate(bool primary)
{
  sensorDS18B20int.topicsCreate(primary);
  sensorDS18B20ext.topicsCreate(primary);
  sensorSHT30.topicsCreate(primary);
  #if CONFIG_RS485_ENABLED
  sensorRS485TH.topicsCreate(primary);
  #endif // CONFIG_RS485_ENABLED
}

void sensorsMqttTopicsFree()
{
  sensorDS18B20int.topicsFree();
  sensorDS18B20ext.topicsFree();
  sensorSHT30.topicsFree();
  #if CONFIG_RS485_ENABLED
  sensorRS485TH.topicsFree();
  #endif // CONFIG_RS485_ENABLED
}

void sensorsMqttPublish()
{
  sensorDS18B20int.publishData(false);
  sensorDS18B20ext.publishData(false);
  sensorSHT30.publishData(false);
  #if CONFIG_RS485_ENABLED
  sensorRS485TH.publishData(false);
  #endif // CONFIG_RS485_ENABLED
}

void sensorsStoreData()
{
  sensorDS18B20int.nvsStoreExtremums(SENSOR_DS18B20INT_KEY);
  sensorDS18B20ext.nvsStoreExtremums(SENSOR_DS18B20EXT_KEY);
  sensorSHT30.nvsStoreExtremums(SENSOR_SHT30_KEY);
  #if CONFIG_RS485_ENABLED
  sensorRS485TH.nvsStoreExtremums(SENSOR_RS485TH_KEY);
  #endif // CONFIG_RS485_ENABLED
}

#if CONFIG_RS485_ENABLED

void sensorsInitModbus()
{
  rlog_i(logTAG, "Modbus initialization");
  // Инициализация RS485 и Modbus
  RE_OK_CHECK_EVENT(mbc_master_init(MB_PORT_SERIAL_MASTER, &_modbus), return);
  // Configure Modbus
  mb_communication_info_t comm;
  memset(&comm, 0, sizeof(comm));
  comm.mode = MB_MODE_RTU;
  comm.port = CONFIG_MODBUS_PORT;
  comm.baudrate = CONFIG_MODBUS_SPEED;
  comm.parity = UART_PARITY_DISABLE;
  RE_OK_CHECK_EVENT(mbc_master_setup((void*)&comm), return);
  // Set UART pins
  RE_OK_CHECK_EVENT(uart_set_pin(CONFIG_MODBUS_PORT, CONFIG_GPIO_RS485_TX, CONFIG_GPIO_RS485_RX, CONFIG_GPIO_RS485_RTS, CONFIG_GPIO_RS485_CTS), return);
  // Start Modbus
  RE_OK_CHECK_EVENT(mbc_master_start(), return);
  // Set UART mode
  RE_OK_CHECK_EVENT(uart_set_mode(CONFIG_MODBUS_PORT, UART_MODE_RS485_HALF_DUPLEX), return);
}

#endif // CONFIG_RS485_ENABLED

void sensorsInitSensors()
{
  sensorDS18B20int.setSensorItems(&siDS18B20intTemp);
  sensorDS18B20int.registerParameters(pgSensors, SENSOR_DS18B20INT_KEY, SENSOR_DS18B20INT_TOPIC, SENSOR_DS18B20INT_NAME);
  sensorDS18B20int.nvsRestoreExtremums(SENSOR_DS18B20INT_KEY);
  sensorDS18B20int.sensorStart();

  sensorDS18B20ext.setSensorItems(&siDS18B20extTemp);
  sensorDS18B20ext.registerParameters(pgSensors, SENSOR_DS18B20EXT_KEY, SENSOR_DS18B20EXT_TOPIC, SENSOR_DS18B20EXT_NAME);
  sensorDS18B20ext.nvsRestoreExtremums(SENSOR_DS18B20EXT_KEY);
  sensorDS18B20ext.sensorStart();

  sensorSHT30.setSensorItems(&siSHT30Humd, &siSHT30Temp);
  sensorSHT30.registerParameters(pgSensors, SENSOR_SHT30_KEY, SENSOR_SHT30_TOPIC, SENSOR_SHT30_NAME);
  sensorSHT30.nvsRestoreExtremums(SENSOR_SHT30_KEY);
  sensorSHT30.sensorStart();

  #if CONFIG_RS485_ENABLED
  sensorRS485TH.setSensorItems(&siRS485Humd, &siRS485Temp);
  sensorRS485TH.registerParameters(pgSensors, SENSOR_RS485TH_KEY, SENSOR_RS485TH_TOPIC, SENSOR_RS485TH_NAME);
  sensorRS485TH.nvsRestoreExtremums(SENSOR_RS485TH_KEY);
  sensorRS485TH.sensorStart();
  #endif // CONFIG_RS485_ENABLED
}

void sensorsSensorsResetExtremumsAll(uint8_t mode)
{
  switch (mode) {
    case 1:
      sensorDS18B20int.resetExtremumsDaily();
      sensorDS18B20ext.resetExtremumsDaily();
      sensorSHT30.resetExtremumsDaily();
      #if CONFIG_RS485_ENABLED
      sensorRS485TH.resetExtremumsDaily();
      #endif // CONFIG_RS485_ENABLED
      break;
    case 2:
      sensorDS18B20int.resetExtremumsWeekly();
      sensorDS18B20ext.resetExtremumsWeekly();
      sensorSHT30.resetExtremumsWeekly();
      #if CONFIG_RS485_ENABLED
      sensorRS485TH.resetExtremumsWeekly();
      #endif // CONFIG_RS485_ENABLED
      break;
    case 3:
      sensorDS18B20int.resetExtremumsEntirely();
      sensorDS18B20ext.resetExtremumsEntirely();
      sensorSHT30.resetExtremumsEntirely();
      #if CONFIG_RS485_ENABLED
      sensorRS485TH.resetExtremumsEntirely();
      #endif // CONFIG_RS485_ENABLED
      break;
    default:
      sensorDS18B20int.resetExtremumsTotal();
      sensorDS18B20ext.resetExtremumsTotal();
      sensorSHT30.resetExtremumsTotal();
      #if CONFIG_RS485_ENABLED
      sensorRS485TH.resetExtremumsTotal();
      #endif // CONFIG_RS485_ENABLED
      break;
  };
}

void sensorsResetExtremumsSensor(rSensor* sensor, const char* sensor_name, uint8_t mode) 
{ 
  if (mode == 0) {
    sensor->resetExtremumsTotal();
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_TOTAL_DEV, sensor_name);
    #endif // CONFIG_TELEGRAM_ENABLE
  } else if (mode == 1) {
    sensor->resetExtremumsDaily();
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_DAILY_DEV, sensor_name);
    #endif // CONFIG_TELEGRAM_ENABLE
  } else if (mode == 2) {
    sensor->resetExtremumsWeekly();
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_WEEKLY_DEV, sensor_name);
    #endif // CONFIG_TELEGRAM_ENABLE
  } else if (mode == 3) {
    sensor->resetExtremumsEntirely();
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_ENTIRELY_DEV, sensor_name);
    #endif // CONFIG_TELEGRAM_ENABLE
  };
}

void sensorsResetExtremumsSensors(uint8_t mode)
{
  if (mode == 0) {
    sensorsSensorsResetExtremumsAll(0);
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_TOTAL_ALL);
    #endif // CONFIG_TELEGRAM_ENABLE
  } else if (mode == 1) {
    sensorsSensorsResetExtremumsAll(1);
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_DAILY_ALL);
    #endif // CONFIG_TELEGRAM_ENABLE
  } else if (mode == 2) {
    sensorsSensorsResetExtremumsAll(2);
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_WEEKLY_ALL);
    #endif // CONFIG_TELEGRAM_ENABLE
  } else if (mode == 3) {
    sensorsSensorsResetExtremumsAll(3);
    #if CONFIG_TELEGRAM_ENABLE
      tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
        CONFIG_MESSAGE_TG_SENSOR_CLREXTR_ENTIRELY_ALL);
    #endif // CONFIG_TELEGRAM_ENABLE
  };
};

bool sensorsSensorsResetExtremumsSelected(char * topic, uint8_t imode)
{
  if (strcasecmp(topic, SENSOR_DS18B20INT_TOPIC) == 0) {
    sensorsResetExtremumsSensor(&sensorDS18B20int, SENSOR_DS18B20INT_TOPIC, imode);
    return true;
  } else if (strcasecmp(topic, SENSOR_DS18B20EXT_TOPIC) == 0) {
    sensorsResetExtremumsSensor(&sensorDS18B20ext, SENSOR_DS18B20EXT_TOPIC, imode);
    return true;
  } else if (strcasecmp(topic, SENSOR_SHT30_TOPIC) == 0) {
    sensorsResetExtremumsSensor(&sensorSHT30, SENSOR_SHT30_TOPIC, imode);
    return true;
  #if CONFIG_RS485_ENABLED
  } else if (strcasecmp(topic, SENSOR_RS485TH_TOPIC) == 0) {
    sensorsResetExtremumsSensor(&sensorRS485TH, SENSOR_RS485TH_TOPIC, imode);
    return true;
  #endif // CONFIG_RS485_ENABLED
  };
  return false;
}

void sensorsReadSensors()
{
  rlog_i(logTAG, "Read data from sensors...");

  sensorDS18B20int.readData();
  if (sensorDS18B20int.getStatus() == SENSOR_STATUS_OK) {
    rlog_i("DS18B20 INT", "Values raw: %.1f °С | out: %.1f °С | min: %.1f °С | max: %.1f °С", 
      sensorDS18B20int.getTemperature(false).rawValue, 
      sensorDS18B20int.getTemperature(false).filteredValue,
      sensorDS18B20int.getItemExtremumsDaily(0, false).minValue.filteredValue,
      sensorDS18B20int.getItemExtremumsDaily(0, false).maxValue.filteredValue);
  };

  sensorDS18B20ext.readData();
  if (sensorDS18B20ext.getStatus() == SENSOR_STATUS_OK) {
    rlog_i("DS18B20 EXT", "Values raw: %.1f °С | out: %.1f °С | min: %.1f °С | max: %.1f °С", 
      sensorDS18B20ext.getTemperature(false).rawValue, 
      sensorDS18B20ext.getTemperature(false).filteredValue,
      sensorDS18B20ext.getItemExtremumsDaily(0, false).minValue.filteredValue,
      sensorDS18B20ext.getItemExtremumsDaily(0, false).maxValue.filteredValue);
  };

  sensorSHT30.readData();
  if (sensorSHT30.getStatus() == SENSOR_STATUS_OK) {
    rlog_i("SHT30", "Values raw: %.2f °С / %.2f %% | out: %.2f °С / %.2f %% | min: %.2f °С / %.2f %% | max: %.2f °С / %.2f %%", 
      sensorSHT30.getTemperature(false).rawValue, sensorSHT30.getHumidity(false).rawValue, 
      sensorSHT30.getTemperature(false).filteredValue, sensorSHT30.getHumidity(false).filteredValue, 
      sensorSHT30.getItemExtremumsDaily(1, false).minValue.filteredValue, sensorSHT30.getItemExtremumsDaily(0, false).minValue.filteredValue, 
      sensorSHT30.getItemExtremumsDaily(1, false).maxValue.filteredValue, sensorSHT30.getItemExtremumsDaily(0, false).maxValue.filteredValue);
  };

  #if CONFIG_RS485_ENABLED
  sensorRS485TH.readData();
  if (sensorRS485TH.getStatus() == SENSOR_STATUS_OK) {
    rlog_i("RS485", "Values raw: %.2f °С / %.2f %% | out: %.2f °С / %.2f %% | min: %.2f °С / %.2f %% | max: %.2f °С / %.2f %%", 
      sensorRS485TH.getTemperature(false).rawValue, sensorRS485TH.getHumidity(false).rawValue, 
      sensorRS485TH.getTemperature(false).filteredValue, sensorRS485TH.getHumidity(false).filteredValue, 
      sensorRS485TH.getItemExtremumsDaily(1, false).minValue.filteredValue, sensorRS485TH.getItemExtremumsDaily(0, false).minValue.filteredValue, 
      sensorRS485TH.getItemExtremumsDaily(1, false).maxValue.filteredValue, sensorRS485TH.getItemExtremumsDaily(0, false).maxValue.filteredValue);
  };
  #endif // CONFIG_RS485_ENABLED
}

// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------- Internal GPIO ----------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

// Обработчик событий по входам
void isrGpioInputsSignal(void* source, gpio_data_t data, uint32_t duration)
{
  rlog_i(logTAG, "Input signal: pin = %d, value = %d", data.pin, data.value);
  if (data.value == 1) {
    if (data.pin == CONFIG_GPIO_DOORBTN_BTN) {
      xEventGroupSetBits(_sensorsFlags, FLG_INPUT_DOORBTN);
    } else if (data.pin == CONFIG_GPIO_GAS) {
      xEventGroupSetBits(_sensorsFlags, FLG_INPUT_GAS);
    }
  };
}

// Подсветка кнопки дверного звонка
static ledQueue_t ledDoorBtn = nullptr;
// Вход дверной кнопки
static reGPIO gpioDoorButton(CONFIG_GPIO_DOORBTN_BTN, true, false, true, CONFIG_BUTTON_DEBOUNCE_TIME_US, isrGpioInputsSignal);
// Вход с сигнализатора утечки газа
static reGPIO gpioGasLeak(CONFIG_GPIO_GAS, true, false, true, CONFIG_BUTTON_DEBOUNCE_TIME_US, isrGpioInputsSignal);
// Реле управления котлом (сухой контакт)
static rLoadGpioController lcBoiler(CONFIG_GPIO_BOILER, 1, false, "blr");

void gpioInit()
{
  // Подсветка кнопки дверного звонка
  ledDoorBtn = ledTaskCreate(CONFIG_GPIO_DOORBTN_LED, true, true, "led_doorbtn", CONFIG_LED_TASK_STACK_SIZE, nullptr);
  ledTaskSend(ledDoorBtn, lmBlinkOn, 1, 150, 5000);

  // Вход дверной кнопки
  gpioDoorButton.initGPIO();
  // Вход с сигнализатора утечки газа
  gpioGasLeak.initGPIO();

  // Выход управления котлом (реле, сухой контакт)
  lcBoiler.loadInit(false);
  lcBoiler.countersNvsRestore();
  /* Если нет желания использовать LoadController, можно использовать код ниже
  gpio_reset_pin((gpio_num_t)CONFIG_GPIO_BOILER);
  gpio_set_direction((gpio_num_t)CONFIG_GPIO_BOILER, GPIO_MODE_OUTPUT);
  gpio_set_pull_mode((gpio_num_t)CONFIG_GPIO_BOILER, GPIO_PULLDOWN_ONLY);
  gpio_set_level((gpio_num_t)CONFIG_GPIO_BOILER, CONFIG_GPIO_LEVEL_OUTPUTS_OFF);
  */
}

void gpioStoreData()
{
  lcBoiler.countersNvsStore();
}

// Дверной звонок с уведомлениями
void gpioDoorButtonPress()
{
  #if CONFIG_TELEGRAM_ENABLE
    tgSend(MK_SECURITY, MP_HIGH, true, CONFIG_TELEGRAM_DEVICE, "Нажата кнопка входного звонка");
  #endif // CONFIG_TELEGRAM_ENABLE
  ledTaskSend(ledDoorBtn, lmFlash, 10, 500, 500);
}

// Уведомления с промышленного датчика утечки газа
void gpioGasLeakSignal()
{
  #if CONFIG_TELEGRAM_ENABLE
    tgSend(MK_SECURITY, MP_HIGH, true, CONFIG_TELEGRAM_DEVICE, "Внимание! Сработало устройство обнаружения утечек газа!");
  #endif // CONFIG_TELEGRAM_ENABLE
}

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------ Термостат ------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

// Простейший термостат (на фиксированную температуру 25 градусов)
void tempControl()
{
  if (sensorDS18B20ext.getStatus() == SENSOR_STATUS_OK) {
    if (sensorDS18B20ext.getTemperature(false).filteredValue < 27.0) {
      rlog_i(logTAG, "Boiler enabled");
      lcBoiler.loadSetState(true, false, true);
      // gpio_set_level((gpio_num_t)CONFIG_GPIO_BOILER, CONFIG_GPIO_LEVEL_OUTPUTS_ON);
    } else {
      rlog_i(logTAG, "Boiler disabled");
      lcBoiler.loadSetState(false, false, true);
      // gpio_set_level((gpio_num_t)CONFIG_GPIO_BOILER, CONFIG_GPIO_LEVEL_OUTPUTS_OFF);
    };
  };
}

// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------- Event Handlers ---------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

void evhdlTimeEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if (event_id == RE_TIME_EVERY_MINUTE) {
    xEventGroupSetBits(_sensorsFlags, FLG_TIME_CHANGED);
  };
  if (event_id == RE_TIME_START_OF_DAY) {
    xEventGroupSetBits(_sensorsFlags, FLG_STORE_DATA);
  };
  lcBoiler.countersTimeEventHandler(event_id, event_data);
}

void evhdlMqttEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if (event_id == RE_MQTT_CONNECTED) {
    re_mqtt_event_data_t* data = (re_mqtt_event_data_t*)event_data;
    sensorsMqttTopicsCreate(data->primary);
  } 
  else if ((event_id == RE_MQTT_CONN_LOST) || (event_id == RE_MQTT_CONN_FAILED)) {
    sensorsMqttTopicsFree();
  }
}

void evhdlParamsEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if (event_id == RE_PARAMS_CHANGED) {
    xEventGroupSetBits(_sensorsFlags, FLG_PARAMS_CHANGED);
  };
}

void evhdlCommandsEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if ((event_id == RE_SYS_COMMAND) && (event_data)) {
    char* buf = malloc_string((char*)event_data);
    if (buf != nullptr) {
      const char* seps = " ";
      char* cmd = nullptr;
      char* mode = nullptr;
      char* sensor = nullptr;
      uint8_t imode = 0;
      cmd = strtok(buf, seps);
      
      // Команды сброса датчиков
      if ((cmd != nullptr) && (strcasecmp(cmd, CONFIG_SENSOR_COMMAND_EXTR_RESET) == 0)) {
        rlog_i(logTAG, "Reset extremums: %s", buf);
        sensor = strtok(nullptr, seps);
        if (sensor != nullptr) {
          mode = strtok(nullptr, seps);
        };
      
        // Опрделение режима сброса
        if (mode == nullptr) {
          // Возможно, вторым токеном идет режим, в этом случае сбрасываем для всех сенсоров
          if (sensor) {
            if (strcasecmp(sensor, CONFIG_SENSOR_EXTREMUMS_DAILY) == 0) {
              sensor = nullptr;
              imode = 1;
            } else if (strcasecmp(sensor, CONFIG_SENSOR_EXTREMUMS_WEEKLY) == 0) {
              sensor = nullptr;
              imode = 2;
            } else if (strcasecmp(sensor, CONFIG_SENSOR_EXTREMUMS_ENTIRELY) == 0) {
              sensor = nullptr;
              imode = 3;
            };
          };
        } else if (strcasecmp(mode, CONFIG_SENSOR_EXTREMUMS_DAILY) == 0) {
          imode = 1;
        } else if (strcasecmp(mode, CONFIG_SENSOR_EXTREMUMS_WEEKLY) == 0) {
          imode = 2;
        } else if (strcasecmp(mode, CONFIG_SENSOR_EXTREMUMS_ENTIRELY) == 0) {
          imode = 3;
        };

        // Определение сенсора
        if ((sensor == nullptr) || (strcasecmp(sensor, CONFIG_SENSOR_COMMAND_SENSORS_PREFIX) == 0)) {
          sensorsResetExtremumsSensors(imode);
        } else {
          if (!sensorsSensorsResetExtremumsSelected(sensor, imode)) {
            rlog_w(logTAG, "Sensor [ %s ] not found", sensor);
            #if CONFIG_TELEGRAM_ENABLE
              tgSend(CONFIG_SENSOR_COMMAND_KIND, CONFIG_SENSOR_COMMAND_PRIORITY, CONFIG_SENSOR_COMMAND_NOTIFY, CONFIG_TELEGRAM_DEVICE,
                CONFIG_MESSAGE_TG_SENSOR_CLREXTR_UNKNOWN, sensor);
            #endif // CONFIG_TELEGRAM_ENABLE
          };
        };
      };
    };
    if (buf != nullptr) free(buf);
  };
}

void evhdlOtaEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if ((event_id == RE_SYS_OTA) && (event_data)) {
    re_system_event_data_t* data = (re_system_event_data_t*)event_data;
    if (data->type == RE_SYS_SET) {
      lcBoiler.loadSetState(false, true, true);
      sensorsTaskSuspend();
    } else {
      sensorsTaskResume();
    };
  };
}

void evhdlStoreData()
{
  sensorsStoreData();
  gpioStoreData();  
}

bool evhdlEventHandlersRegister()
{
  return eventHandlerRegister(RE_MQTT_EVENTS, ESP_EVENT_ANY_ID, &evhdlMqttEventHandler, nullptr) 
      && eventHandlerRegister(RE_TIME_EVENTS, ESP_EVENT_ANY_ID, &evhdlTimeEventHandler, nullptr)
      && eventHandlerRegister(RE_PARAMS_EVENTS, ESP_EVENT_ANY_ID, &evhdlParamsEventHandler, nullptr)
      && eventHandlerRegister(RE_SYSTEM_EVENTS, RE_SYS_COMMAND, &evhdlCommandsEventHandler, nullptr)
      && eventHandlerRegister(RE_SYSTEM_EVENTS, RE_SYS_OTA, &evhdlOtaEventHandler, nullptr);
}

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- Задача --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

void sensorsTaskExec(void *pvParameters)
{
  // -------------------------------------------------------------------------------------------------------
  // Инициализация устройств и сенсоров
  // -------------------------------------------------------------------------------------------------------
  gpioInit();
  sensorsInitModbus();
  sensorsInitSensors();
  espRegisterShutdownHandler(evhdlStoreData); // #2

  // -------------------------------------------------------------------------------------------------------
  // Таймеры публикции данных с сенсоров
  // -------------------------------------------------------------------------------------------------------
  static esp_timer_t mqttPubTimer;
  timerSet(&mqttPubTimer, iMqttPubInterval*1000);
  
  static TickType_t waitTicks = 0;
  static TickType_t readTicks = 0;
  static TickType_t currTicks = 0;
  static EventBits_t bits = 0;
  static uint32_t beepFreq = 0;
  static bool out_state = false;
  while (1) {
    // Ждем тайамута или любого сигнального бита (что наступит первым)
    bits = xEventGroupWaitBits(_sensorsFlags, FLGS_WAIT, pdFALSE, pdFALSE, waitTicks);
    // Фиксируем время начала данного рабочего цикла
    readTicks = xTaskGetTickCount(); 
    // Очищаем сигнальные биты (их состояние мы сохранили в bits)
    xEventGroupClearBits(_sensorsFlags, FLGS_CLEAR);

    // Обработка событий по встроенным входам
    if ((bits & FLG_INPUT_DOORBTN) > 0) gpioDoorButtonPress();
    if ((bits & FLG_INPUT_GAS) > 0) gpioGasLeakSignal();

    // Раз в сутки принудительно сохраням экстремумы с сенсоров в NVS
    if ((bits & FLG_STORE_DATA) > 0) sensorsStoreData();

    // Чтение данных с сенсоров
    sensorsReadSensors();

    // Простейший термостат
    tempControl();

    // Переключаем все выходы на MCP23017
    out_state = !out_state;
    rlog_i(logTAG, "Set new outputs level: %d", out_state);
    ioexpSetLevel(0, out_state);
    ioexpSetLevel(1, out_state);
    ioexpSetLevel(2, out_state);
    ioexpSetLevel(3, out_state);
    ioexpSetLevel(4, out_state);
    ioexpSetLevel(5, out_state);
    ioexpSetLevel(6, out_state);
    ioexpSetLevel(7, out_state);
    ioexpSetLevel(8, out_state);
    ioexpSetLevel(9, out_state);
    ioexpSetLevel(10, out_state);
    ioexpSetLevel(11, out_state);
    ioexpSetLevel(12, out_state);
    ioexpSetLevel(13, out_state);
    ioexpSetLevel(14, out_state);
    ioexpSetLevel(15, out_state);

    // Публикация данных с сенсоров на MQTT брокер
    if (timerTimeout(&mqttPubTimer)) {
      timerSet(&mqttPubTimer, iMqttPubInterval*1000);
      sensorsMqttPublish();

      // Подбор частоты пищалки
      beepFreq = beepFreq + 250;
      if (beepFreq > 10000) beepFreq = 250;
      rlog_i(logTAG, "Beep frequency: %d Hz", beepFreq);
      // beepTaskSend(beepFreq, 0, 250, 1, 1024);
    };

    // Считаем время ожидания следующего цикла
    TickType_t currTicks = xTaskGetTickCount();    
    if ((currTicks - readTicks) >= pdMS_TO_TICKS(iSensorsReadInterval*1000)) {
      waitTicks = 0;
    } else {
      waitTicks = pdMS_TO_TICKS(iSensorsReadInterval*1000) - (currTicks - readTicks);
    };
  };

  vTaskDelete(nullptr);
  espRestart(RR_ERROR);
}

bool sensorsTaskStart()
{
  #if CONFIG_SENSORS_STATIC_ALLOCATION
    static StaticEventGroup_t sensorsFlagsBuffer;
    static StaticTask_t sensorsTaskBuffer;
    static StackType_t sensorsTaskStack[CONFIG_SENSORS_TASK_STACK_SIZE];
    _sensorsFlags = xEventGroupCreateStatic(&sensorsFlagsBuffer);
    _sensorsTask = xTaskCreateStaticPinnedToCore(sensorsTaskExec, sensorsTaskName, 
      CONFIG_SENSORS_TASK_STACK_SIZE, NULL, CONFIG_SENSORS_TASK_PRIORITY, sensorsTaskStack, &sensorsTaskBuffer, CONFIG_SENSORS_TASK_CORE);
  #else
    _sensorsFlags = xEventGroupCreate();
    xTaskCreatePinnedToCore(sensorsTaskExec, sensorsTaskName, 
      CONFIG_SENSORS_TASK_STACK_SIZE, NULL, CONFIG_SENSORS_TASK_PRIORITY, &_sensorsTask, CONFIG_SENSORS_TASK_CORE);
  #endif // CONFIG_SENSORS_STATIC_ALLOCATION
  if (_sensorsFlags && _sensorsTask) {
    rloga_i("Task [ %s ] has been successfully created and started", sensorsTaskName);
    return evhdlEventHandlersRegister();
  }
  else {
    rloga_e("Failed to create a task for processing sensor readings!");
    return false;
  };
}

bool sensorsTaskSuspend()
{
  if ((_sensorsTask) && (eTaskGetState(_sensorsTask) != eSuspended)) {
    vTaskSuspend(_sensorsTask);
    if (eTaskGetState(_sensorsTask) == eSuspended) {
      rloga_d("Task [ %s ] has been suspended", sensorsTaskName);
      return true;
    } else {
      rloga_e("Failed to suspend task [ %s ]!", sensorsTaskName);
    };
  };
  return false;
}

bool sensorsTaskResume()
{
  if ((_sensorsTask) && (eTaskGetState(_sensorsTask) == eSuspended)) {
    vTaskResume(_sensorsTask);
    if (eTaskGetState(_sensorsTask) != eSuspended) {
      rloga_i("Task [ %s ] has been successfully resumed", sensorsTaskName);
      return true;
    } else {
      rloga_e("Failed to resume task [ %s ]!", sensorsTaskName);
    };
  };
  return false;
}
