#ifndef __IOEXP_H__
#define __IOEXP_H__

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "project_config.h"
#include "def_consts.h"

#define CONFIG_RESET_SMOKE_TIMEOUT_S  5

typedef enum {
  IOEXP_LVL_0 = 0,  // Установить 0 на выходе
  IOEXP_LVL_1 = 1,  // Установить 1 на выходе
  IOEXP_TIMER = 2,  // Установить 1 на выходе, а через 5 секунд установить 0
  IOEXP_RESET = 3,  // Аппаратный сброс всех расширителей
  IOEXP_ISR_1 = 4,  // Прерывание на расширителе 1
  IOEXP_ISR_2 = 5,  // Прерывание на расширителе 2
  IOEXP_ISR_3 = 6,  // Прерывание на расширителе 3
  IOEXP_ISR_4 = 7   // Прерывание на расширителе 4
} ioexp_type_t;

typedef struct {
  ioexp_type_t type;
  uint8_t pin;
} ioexp_command_t;

bool ioexpInit();
bool ioexpTaskStart();

bool ioexpSetLevel(uint8_t pin, bool level);
bool ioexpResetTimer(uint8_t pin);
bool ioexpResetMcp();


#endif // __IOEXP_H__