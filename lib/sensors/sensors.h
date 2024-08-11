#ifndef __SENSORS_H__
#define __SENSORS_H__

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "project_config.h"
#include "def_consts.h"
#include "esp_timer.h"
#include "rTypes.h" 
#include "reSensor.h" 
#include "reDS18x20.h"
#include "reTH485.h"
#include "reSHT3x.h"

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- Сенсоры -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

bool sensorsPublish(rSensor *sensor, char* topic, char* payload, const bool free_topic, const bool free_payload);

#define SENSORS_DEFAULT_READ_INTERVAL       10    // s
#define SENSORS_MINIMAL_READ_INTERVAL       1000  // ms
#define SENSORS_TOPIC_LOCAL                 0
#define SENSORS_FILTER_MODE                 SENSOR_FILTER_RAW
#define SENSORS_FILTER_SIZE                 0
#define SENSORS_ERRORS_LIMIT                16

// Период опроса датчиков (чтения данных с сенсоров) в секундах
static uint32_t iSensorsReadInterval = SENSORS_DEFAULT_READ_INTERVAL;
// Период публикации данных с сенсоров на MQTT
static uint32_t iMqttPubInterval = CONFIG_MQTT_SENSORS_SEND_INTERVAL;

// Указатель на структуру modbus master
static void* _modbus = nullptr;

// DS18B20, расположенный на плате (проверка шины 1-Wire #1)
#define SENSOR_DS18B20INT_IDS               1
#define SENSOR_DS18B20INT_NAME              "DS18B20 на плате"
#define SENSOR_DS18B20INT_KEY               "int"
#define SENSOR_DS18B20INT_TOPIC             "ds18b20_int"
#define SENSOR_DS18B20INT_INDEX             1
#define SENSOR_DS18B20INT_ADDR              ONEWIRE_NONE
#define SENSOR_DS18B20INT_PIN               (gpio_num_t)CONFIG_GPIO_1WIRE_DS1820
#define SENSOR_DS18B20INT_RESOLUTION        DS18x20_RESOLUTION_12_BIT
#define SENSOR_DS18B20INT_READ_SCRATCHPAD   true

static DS18x20 sensorDS18B20int(SENSOR_DS18B20INT_IDS, 
  SENSOR_DS18B20INT_PIN, SENSOR_DS18B20INT_ADDR, SENSOR_DS18B20INT_INDEX, SENSOR_DS18B20INT_RESOLUTION, SENSOR_DS18B20INT_READ_SCRATCHPAD,
  SENSOR_DS18B20INT_NAME, SENSOR_DS18B20INT_TOPIC, SENSORS_TOPIC_LOCAL, SENSORS_MINIMAL_READ_INTERVAL, SENSORS_ERRORS_LIMIT,
  nullptr, sensorsPublish);
static rTemperatureItem siDS18B20intTemp(&sensorDS18B20int, 
  CONFIG_SENSOR_TEMP_KEY, CONFIG_SENSOR_TEMP_NAME, CONFIG_SENSOR_TEMP_FRIENDLY, CONFIG_FORMAT_TEMP_UNIT,
  SENSORS_FILTER_MODE, SENSORS_FILTER_SIZE, 
  CONFIG_FORMAT_TEMP_VALUE, CONFIG_FORMAT_TEMP_STRING);

// DS18B20 подключенный к разъему TM  (проверка шины 1-Wire #2)
#define SENSOR_DS18B20EXT_IDS               2
#define SENSOR_DS18B20EXT_NAME              "DS18B20 на разъеме ТМ"
#define SENSOR_DS18B20EXT_KEY               "ext"
#define SENSOR_DS18B20EXT_TOPIC             "ds18b20_ext"
#define SENSOR_DS18B20EXT_INDEX             1
#define SENSOR_DS18B20EXT_ADDR              ONEWIRE_NONE
#define SENSOR_DS18B20EXT_PIN               (gpio_num_t)CONFIG_GPIO_1WIRE_TM
#define SENSOR_DS18B20EXT_RESOLUTION        DS18x20_RESOLUTION_12_BIT
#define SENSOR_DS18B20EXT_READ_SCRATCHPAD   true

static DS18x20 sensorDS18B20ext(SENSOR_DS18B20EXT_IDS, 
  SENSOR_DS18B20EXT_PIN, SENSOR_DS18B20EXT_ADDR, SENSOR_DS18B20EXT_INDEX, SENSOR_DS18B20EXT_RESOLUTION, SENSOR_DS18B20EXT_READ_SCRATCHPAD,
  SENSOR_DS18B20EXT_NAME, SENSOR_DS18B20EXT_TOPIC, SENSORS_TOPIC_LOCAL, SENSORS_MINIMAL_READ_INTERVAL, SENSORS_ERRORS_LIMIT,
  nullptr, sensorsPublish);
static rTemperatureItem siDS18B20extTemp(&sensorDS18B20ext, 
  CONFIG_SENSOR_TEMP_KEY, CONFIG_SENSOR_TEMP_NAME, CONFIG_SENSOR_TEMP_FRIENDLY, CONFIG_FORMAT_TEMP_UNIT,
  SENSORS_FILTER_MODE, SENSORS_FILTER_SIZE, 
  CONFIG_FORMAT_TEMP_VALUE, CONFIG_FORMAT_TEMP_STRING);

// SHT30 подключенный к разъему I2C#1  (проверка шины I2C#1)
#define SENSOR_SHT30_IDS                    3
#define SENSOR_SHT30_NAME                   "SHT30 на шине I2C#1"
#define SENSOR_SHT30_KEY                    "i2c"
#define SENSOR_SHT30_TOPIC                  "sht30"
#define SENSOR_SHT30_BUS                    I2C_NUM_1
#define SENSOR_SHT30_ADDRESS                SHT3xD_ADDRESS_1
#define SENSOR_SHT30_MODE                   SHT3xD_MODE_NOHOLD
#define SENSOR_SHT30_REPEATABILITY          SHT3xD_REPEATABILITY_HIGH
#define SENSOR_SHT30_FREQUENCY              SHT3xD_SINGLE

static SHT3xD sensorSHT30(SENSOR_SHT30_IDS, 
  SENSOR_SHT30_BUS, SENSOR_SHT30_ADDRESS, SENSOR_SHT30_FREQUENCY, SENSOR_SHT30_MODE, SENSOR_SHT30_REPEATABILITY, 
  SENSOR_SHT30_NAME, SENSOR_SHT30_TOPIC, SENSORS_TOPIC_LOCAL, SENSORS_MINIMAL_READ_INTERVAL, SENSORS_ERRORS_LIMIT,
  nullptr, sensorsPublish);
static rTemperatureItem siSHT30Temp(&sensorSHT30, 
  CONFIG_SENSOR_TEMP_KEY, CONFIG_SENSOR_TEMP_NAME, CONFIG_SENSOR_TEMP_FRIENDLY, CONFIG_FORMAT_TEMP_UNIT,
  SENSORS_FILTER_MODE, SENSORS_FILTER_SIZE, 
  CONFIG_FORMAT_TEMP_VALUE, CONFIG_FORMAT_TEMP_STRING);
static rSensorItem siSHT30Humd(&sensorSHT30, 
  CONFIG_SENSOR_HUMIDITY_KEY, CONFIG_SENSOR_HUMIDITY_NAME, CONFIG_SENSOR_HUMIDITY_FRIENDLY, 
  SENSORS_FILTER_MODE, SENSORS_FILTER_SIZE, 
  CONFIG_FORMAT_HUMIDITY_VALUE, CONFIG_FORMAT_HUMIDITY_STRING);

#if CONFIG_RS485_ENABLED

// Любой RS485 сенсор, подключенный к шине RS485 (проверка шины RS485)
#define SENSOR_RS485TH_IDS                  3
#define SENSOR_RS485TH_NAME                 "TH сенсор на RS485"
#define SENSOR_RS485TH_KEY                  "mod"
#define SENSOR_RS485TH_TOPIC                "rs485th"
#define SENSOR_RS485TH_ADDRESS              0x01
#define SENSOR_RS485TH_READ_CMD             0x03
#define SENSOR_RS485TH_REG_TEMP             0x0001
#define SENSOR_RS485TH_REG_HUMD             0x0000

static reTH485 sensorRS485TH(SENSOR_RS485TH_IDS, 
  &_modbus, SENSOR_RS485TH_ADDRESS, SENSOR_RS485TH_READ_CMD, SENSOR_RS485TH_REG_TEMP, SENSOR_RS485TH_REG_HUMD,
  SENSOR_RS485TH_NAME, SENSOR_RS485TH_TOPIC, SENSORS_TOPIC_LOCAL, SENSORS_MINIMAL_READ_INTERVAL, SENSORS_ERRORS_LIMIT,
  nullptr, sensorsPublish);
static rTemperatureItem siRS485Temp(&sensorRS485TH, 
  CONFIG_SENSOR_TEMP_KEY, CONFIG_SENSOR_TEMP_NAME, CONFIG_SENSOR_TEMP_FRIENDLY, CONFIG_FORMAT_TEMP_UNIT,
  SENSORS_FILTER_MODE, SENSORS_FILTER_SIZE, 
  CONFIG_FORMAT_TEMP_VALUE, CONFIG_FORMAT_TEMP_STRING);
static rSensorItem siRS485Humd(&sensorRS485TH, 
  CONFIG_SENSOR_HUMIDITY_KEY, CONFIG_SENSOR_HUMIDITY_NAME, CONFIG_SENSOR_HUMIDITY_FRIENDLY, 
  SENSORS_FILTER_MODE, SENSORS_FILTER_SIZE, 
  CONFIG_FORMAT_HUMIDITY_VALUE, CONFIG_FORMAT_HUMIDITY_STRING);

#endif // CONFIG_RS485_ENABLED

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- Задача --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

bool sensorsTaskStart();
bool sensorsTaskSuspend();
bool sensorsTaskResume();

#endif // __SENSORS_H__