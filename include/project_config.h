/*
   -----------------------------------------------------------------------------------------------------------------------
   EN: Project configuration file, accessible from all libraries connected to the project
   RU: Файл конфигурации проекта, он должен быть доступен из всех файлов проекта, в том числе и библиотек
   -----------------------------------------------------------------------------------------------------------------------
   (с) 2024 Разживин Александр | Razzhivin Alexander
   kotyara12@yandex.ru | https://kotyara12.ru | tg: @kotyara1971
*/

#pragma once

#include <stdint.h>
#include "esp_task.h"

// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------- EN - Versions ----------------------------------------------------
// ----------------------------------------------------- RU - Версии -----------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
#define APP_VERSION "20240801.001"
// 20240801.001: Базовая версия

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------- EN - Internal GPIO --------------------------------------------------
// ------------------------------------------------ RU - Встроенные GPIO -------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// #define CONFIG_GPIO_SYSTEM_LED      23

// EN: Logic levels on GPIO
// RU: Логические уровни на GPIO
#define CONFIG_GPIO_LEVEL_ESP_ENABLED     1
#define CONFIG_GPIO_LEVEL_ESP_DISABLED    0

// EN: Execution devices connected to ESP32
// RU: Исполнительные устройства, подключенные к ESP32
#define CONFIG_GPIO_BUZZER                12
#define CONFIG_GPIO_BOILER                13

// EN: ESP32 digital inputs
// RU: Цифровые входы ESP32
#define CONFIG_GPIO_RX433                 23
#define CONFIG_GPIO_GAS                   35
#define CONFIG_GPIO_PSA_CHECK             36          
#define CONFIG_GPIO_AKK_VOLTAGE           39

// EN: Doorbell: Illuminated push button
// RU: Дверной звонок: кнопка с подсветкой
#define CONFIG_GPIO_DOORBTN_LED           2
#define CONFIG_GPIO_DOORBTN_BTN           34

// EN: GPIO expanders MCP23017
// RU: Расширители GPIO MCP23017
#define CONFIG_GPIO_IOEXP_RESET           4
#define CONFIG_GPIO_IOEXP_ISR1            18
#define CONFIG_GPIO_IOEXP_ISR2            19

// EN: 1-Wire buses
// RU: Шины 1-Wire
#define CONFIG_GPIO_1WIRE_TM              5
#define CONFIG_GPIO_1WIRE_DS1820          14

// EN: RS485 bus 
// RU: Шина RS485
#define CONFIG_RS485_ENABLED              1
#define CONFIG_GPIO_RS485_RX              25
#define CONFIG_GPIO_RS485_TX              32
#define CONFIG_GPIO_RS485_RTS             33
#define CONFIG_GPIO_RS485_CTS             -1
#define CONFIG_MODBUS_PORT                UART_NUM_1
#define CONFIG_MODBUS_SPEED               9600

// EN: OpenTherm bus 
// RU: Шина OpenTherm
#define CONFIG_OPENTHERM_ENABLED          1
#define CONFIG_GPIO_OPENTHERM_RX          26
#define CONFIG_GPIO_OPENTHERM_TX          27

// EN: I2C bus #0 (GPIO expanders): pins, pullup, frequency
// RU: Шина I2C #0 (расширители GPIO): выводы, подтяжка, частота
#define CONFIG_I2C_PORT0_SDA              21
#define CONFIG_I2C_PORT0_SCL              22
#define CONFIG_I2C_PORT0_PULLUP           false 
#define CONFIG_I2C_PORT0_FREQ_HZ          100000
#define CONFIG_I2C_PORT0_STATIC           2

// EN: I2C bus #1 (external sensors): pins, pullup, frequency
// RU: Шина I2C #1 (внешние сенсоры): выводы, подтяжка, частота
#define CONFIG_I2C_PORT1_SDA              15
#define CONFIG_I2C_PORT1_SCL              0
#define CONFIG_I2C_PORT1_PULLUP           false
#define CONFIG_I2C_PORT1_FREQ_HZ          100000
#define CONFIG_I2C_PORT1_STATIC           2

// EN: Blocking access to I2C buses. Makes sense if I2C devices are accessed from different threads
// RU: Блокировка доступа к шинам I2C. Имеет смысл, если доступ к устройствам I2C осущствляется из разных потоков
#define CONFIG_I2C_LOCK                   1

// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------- EN - GPIO expanders MCP23017 ---------------------------------------------
// ------------------------------------------- RU - Расширители GPIO MCP23017 --------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

// EN: MCP23017 chip on main board
// RU: Микросхема MCP23017 на основной плате (работает только "на выход")
#define CONFIG_IOEXP_OUTPUTS_BUS          I2C_NUM_0
#define CONFIG_IOEXP_OUTPUTS_ADDRESS      0x20

// EN: Logical levels at outputs
// RU: Логические уровни на выходах
#define CONFIG_IOEXP_OUTPUTS_LEVEL_LED    0
#define CONFIG_IOEXP_OUTPUTS_LEVEL_RELAY  1

// EN: Doorbell 12V
// RU: Дверной звонок 12В
#define CONFIG_IOEXP_OUTPUTS_DOORBELL     0x00 
// EN: Siren 12V
// RU: Сирена 12В
#define CONFIG_IOEXP_OUTPUTS_SIREN        0x01
// EN: Flasher 12V
// RU: Световой извещатель 12В
#define CONFIG_IOEXP_OUTPUTS_FLASHER      0x02
// EN: External relay or 12V load (MOSFET open drain)
// RU: Внешнее реле или нагрузка 12В (MOSFET open drain)
#define CONFIG_IOEXP_OUTPUTS_RELAY_1      0x03
// EN: External relay or 12V load (MOSFET open drain)
// RU: Внешнее реле или нагрузка 12В (MOSFET)
#define CONFIG_IOEXP_OUTPUTS_RELAY_2      0x04
// EN: External relay or 12V load (MOSFET open drain)
// RU: Внешнее реле или нагрузка 12В (MOSFET)
#define CONFIG_IOEXP_OUTPUTS_RELAY_3      0x05
// EN: Reset relay for self-locking two-wire smoke detectors
// RU: Реле сброса самоблокирующихся двухпроводных датчиков дыма
#define CONFIG_IOEXP_OUTPUTS_SMRESET      0x06
// EN: External relay or 12V load (MOSFET open drain)
// RU: Внешнее реле или нагрузка 12В (MOSFET)
#define CONFIG_IOEXP_OUTPUTS_RELAY_4      0x07
// EN: External relay or 12V load (MOSFET open drain)
// RU: Внешнее реле или нагрузка 12В (MOSFET)
#define CONFIG_IOEXP_OUTPUTS_RELAY_5      0x08
// EN: Reserve LED (low level)
// RU: Резерв светодиод (низкий уровень)
#define CONFIG_IOEXP_OUTPUTS_LED_4        0x09
// EN: External relay or 12V load (MOSFET open drain)
// RU: Внешнее реле или нагрузка 12В (MOSFET)
#define CONFIG_IOEXT_OUTPUTS_RELAY_6      0x0A
// EN: System LED (low level)
// RU: Системный светодиод (низкий уровень)
#define CONFIG_IOEXP_OUTPUTS_LED_SYS      0x0B
// EN: External relay or 12V load (MOSFET open drain)
// RU: Внешнее реле или нагрузка 12В (MOSFET)
#define CONFIG_IOEXT_OUTPUTS_RELAY_7      0x0C
// EN: Alarm LED (red) (low level)
// RU: Сигнализация (красный) (низкий уровень)
#define CONFIG_IOEXP_OUTPUTS_LED_ALARM    0x0D
// EN: Battery CTC activation relay
// RU: Реле активации КТЦ аккумулятора
#define CONFIG_IOEXP_OUTPUTS_AKK_CTC      0x0E
// EN: Warning LED (yellow) (low level)
// RU: Предупреждение (желтый) (низкий уровень)
#define CONFIG_IOEXP_OUTPUTS_LED_WARN     0x0F

#define CONFIG_IOEXP_INPUTS1_ADDRESS      0x22
#define CONFIG_IOEXP_INPUTS2_ADDRESS      0x21

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------- EN - Common parameters ----------------------------------------------------
// -------------------------------------------- RU - Общие параметры -----------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

// EN: Default task priority
// RU: Приоритет задачи "по умолчанию"
#define CONFIG_DEFAULT_TASK_PRIORITY 5

// EN: Event loop
// RU: Цикл событий
#define CONFIG_EVENT_LOOP_STACK_SIZE 3072
#define CONFIG_EVENT_LOOP_QUEUE_SIZE 32
#define CONFIG_EVENT_LOOP_CORE 1

/* Silent mode (no sounds, no blinks) */
// EN: Allow "quiet" mode. Quiet mode is the period of time of day when LED flashes and sounds are blocked.
// RU: Разрешить "тихий" режим. Тихий режим - это период времени суток, когда блокируются вспышки светодиодов и звуки.
#define CONFIG_SILENT_MODE_ENABLE 1
// EN: Interval in H1M1H2M2 format. That is, the interval 21:00 -> 06:00 is 21000600
// RU: Интервал в формате H1M1H2M2. То есть интервал 21:00 -> 06:00 это 21000600
#define CONFIG_SILENT_MODE_INTERVAL 21000600 

// EN: Real time clock
// RU: Часы реального времени
#define CONFIG_RTC_INSTALLED 1
#define CONFIG_RTC_TYPE DS3231
#define CONFIG_RTC_I2C_BUS I2C_NUM_0

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------ EN - Application tasks -----------------------------------------------
// ------------------------------------------------ RU - Прикладные задачи -----------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// EN: Here you can specify any parameters related to the main task of the device
// RU: Здесь можно указать вообще любые параметры, связанные с прикладной задачей устройства

// EN: Use static memory allocation for the task and queue. CONFIG_FREERTOS_SUPPORT_STATIC_ALLOCATION must be enabled!
// RU: Использовать статическое выделение памяти под задачу и очередь. Должен быть включен параметр CONFIG_FREERTOS_SUPPORT_STATIC_ALLOCATION!
#define CONFIG_SENSORS_STATIC_ALLOCATION 1
#define CONFIG_IOEXP_STATIC_ALLOCATION 1
// EN: Stack size for application tasks
// RU: Размер стека для прикладных задач
#define CONFIG_SENSORS_TASK_STACK_SIZE 6*1024
#define CONFIG_IOEXP_TASK_STACK_SIZE 4*1024
// EN: Priority of application tasks
// RU: Приоритет прикладных задач
#define CONFIG_SENSORS_TASK_PRIORITY CONFIG_DEFAULT_TASK_PRIORITY+2
#define CONFIG_IOEXP_TASK_PRIORITY CONFIG_DEFAULT_TASK_PRIORITY+2
// EN: Processor core of application tasks
// RU: Процессорное ядро прикладных задач
#define CONFIG_SENSORS_TASK_CORE 1
#define CONFIG_IOEXP_TASK_CORE 1
// EN: Input queue size for GPIO expander control
// RU: Размер входящей очереди для управления расширителями GPIO
#define CONFIG_IOEXP_QUEUE_SIZE 32
#define CONFIG_IOEXP_QUEUE_TIMEOUT 1000

// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------- EN - Sensors -------------------------------------------------------
// -------------------------------------------------- RU - Сенсоры -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

// EN: Allow the publication of sensor data in a simple form (each value in a separate subtopic)
// RU: Разрешить публикацию данных сенсора в простом виде (каждое значение в отдельном субтопике)
#define CONFIG_SENSOR_AS_PLAIN 0
// EN: Allow the publication of sensor data as JSON in one topic
// RU: Разрешить публикацию данных сенсора в виде JSON в одном топике
#define CONFIG_SENSOR_AS_JSON 1
// EN: Allow publication of sensor status
// RU: Разрешить публикацию статуса сенсора
#define CONFIG_SENSOR_STATUS_ENABLE 1

// EN: Allow publishing of raw RAW data (no correction or filtering): 0 - only processed value, 1 - always both values, 2 - only when there is processing
// RU: Разрешить публикацию необработанных RAW-данных (без коррекции и фильтрации): 0 - только обработанное значение, 1 - всегда оба значения, 2 - только когда есть обработка
#define CONFIG_SENSOR_RAW_ENABLE 1
// EN: Allow publication of sensor status
// RU: Разрешить публикацию форматированных данных в виде строки
#define CONFIG_SENSOR_STRING_ENABLE 1
// EN: Allow the publication of the time stamp of reading data from the sensor
// RU: Разрешить публикацию отметки времени чтения данных с сенсора
#define CONFIG_SENSOR_TIMESTAMP_ENABLE 1
// EN: Allow the publication of formatted data as "value + time"
// RU: Разрешить публикацию форматированных данных в виде "значение + время"
#define CONFIG_SENSOR_TIMESTRING_ENABLE 1
// EN: Allow dew point calculation and publication
// RU: Разрешить вычисление и публикацию точки росы
#define CONFIG_SENSOR_DEWPOINT_ENABLE 0
// EN: Allow publishing of mixed value, for example "temperature + humidity"
// RU: Разрешить публикацию смешанного значения, например "температура + влажность"
#define CONFIG_SENSOR_DISPLAY_ENABLED 1
// EN: Allow publication of absolute minimum and maximum
// RU: Разрешить публикацию абсолютного минимума и максимума
#define CONFIG_SENSOR_EXTREMUMS_ENTIRELY_ENABLE 1
// EN: Allow publication of daily minimum and maximum
// RU: Разрешить публикацию ежедневного минимума и максимума
#define CONFIG_SENSOR_EXTREMUMS_DAILY_ENABLE 1
// EN: Allow publication of weekly minimum and maximum
// RU: Разрешить публикацию еженедельного минимума и максимума
#define CONFIG_SENSOR_EXTREMUMS_WEEKLY_ENABLE 1
// EN: Publish extremums only when they are changed
// RU: Публиковать экстеремумы только при их изменении
#define CONFIG_SENSOR_EXTREMUMS_OPTIMIZED 1

// EN: Publish timestamps for turning on and off the load
// RU: Публиковать отметки времени включения и выключения нагрузки
#define CONFIG_LOADCTRL_TIMESTAMP_ENABLED 1
// EN: Publish load on counters
// RU: Публиковать счетчики включения нагрузки
#define CONFIG_LOADCTRL_COUNTERS_ENABLED 1
// EN: Publish load on duration counters
// RU: Публиковать счетчики длительности включения нагрузки
#define CONFIG_LOADCTRL_DURATIONS_ENABLED 1

// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------- EN - Wifi networks -----------------------------------------------------
// ------------------------------------------------ RU - WiFi сети -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
#define CONFIG_WIFI_1_SSID "TEST"
#define CONFIG_WIFI_1_PASS "12346578"

// EN: WiFi connection parameters. Commenting out these lines will use the default ESP-IDF parameters
// RU: Параметры WiFi подключения. Если закомментировать эти строки, будут использованы параметры по умолчанию ESP-IDF
// #define CONFIG_WIFI_STORAGE   WIFI_STORAGE_RAM
// #define CONFIG_WIFI_BANDWIDTH WIFI_BW_HT20

// EN: Restart the device if there is no WiFi connection for more than the specified time in minutes.
//     Comment out the line if you do not need to restart the device if there is no network connection for a long time
// RU: Перезапустить устройство, если нет подключения к WiFi более указанного времени в минутах. 
//     Закомментируйте строку, если не нужно перезапускать устройство при длительном отсутствии подключения к сети
#define CONFIG_WIFI_TIMER_RESTART_DEVICE 60*12

// EN: Allow periodic check of Internet availability using ping.
//     Sometimes network access may be lost, but WiFi connection works. In this case, the device will suspend all network processes.
// RU: Разрешить периодическую проверку доступности сети интернет с помошью пинга. 
//     Иногда доступ в сеть может пропасть, но подключение к WiFi при этом работает. В этом случае устройство приостановит все сетевые процессы.
#define CONFIG_PINGER_ENABLE 1

// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------- EN - MQTT broker -------------------------------------------------------
// ---------------------------------------------- RU - MQTT брокер -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// EN: Parameters of MQTT brokers. Two brokers can be defined: primary and backup
// RU: Параметры MQTT-брокеров. Можно определить два брокера: основной и резервный
// CONFIG_MQTTx_TYPE :: 0 - public, 1 - local, 2 - gateway (CONFIG_MQTT1_HOST not used)

/********************* primary server ************************/
#define CONFIG_MQTT1_TYPE 0
#define CONFIG_MQTT1_HOST "m99.wqtt.ru"
#define CONFIG_MQTT1_PING_CHECK 0
#define CONFIG_MQTT1_PING_CHECK_DURATION 1000
#define CONFIG_MQTT1_PING_CHECK_LOSS 50.0
#define CONFIG_MQTT1_PING_CHECK_LIMIT 5
#define CONFIG_MQTT1_PORT_TCP 2632
#define CONFIG_MQTT1_PORT_TLS 2633
#define CONFIG_MQTT1_USERNAME "u_aaaaaa"
#define CONFIG_MQTT1_PASSWORD "bbbbbbbb"
#define CONFIG_MQTT1_TLS_ENABLED 1
#define CONFIG_MQTT1_TLS_STORAGE TLS_CERT_BUFFER
#define CONFIG_MQTT1_TLS_PEM_START CONFIG_DEFAULT_TLS_PEM_START
#define CONFIG_MQTT1_TLS_PEM_END CONFIG_DEFAULT_TLS_PEM_END
#define CONFIG_MQTT1_CLEAN_SESSION 1
#define CONFIG_MQTT1_AUTO_RECONNECT 1
#define CONFIG_MQTT1_KEEP_ALIVE 60
#define CONFIG_MQTT1_TIMEOUT 10000
#define CONFIG_MQTT1_RECONNECT 10000
#define CONFIG_MQTT1_CLIENTID "esp32_homectrl"
// #define CONFIG_MQTT1_LOC_PREFIX ""
// #define CONFIG_MQTT1_PUB_PREFIX ""
#define CONFIG_MQTT1_LOC_LOCATION "demo/local"
#define CONFIG_MQTT1_PUB_LOCATION "demo"
// #define CONFIG_MQTT1_LOC_DEVICE ""
#define CONFIG_MQTT1_PUB_DEVICE "home_controller"

/****************** MQTT : pinger ********************/
// EN: Allow the publication of ping results on the MQTT broker
// RU: Разрешить публикацию результатов пинга на MQTT брокере
#define CONFIG_MQTT_PINGER_ENABLE 1
#if CONFIG_MQTT_PINGER_ENABLE
// EN: Ping results topic name
// RU: Название топика результатов пинга
#define CONFIG_MQTT_PINGER_TOPIC "ping"
#define CONFIG_MQTT_PINGER_LOCAL 0
#define CONFIG_MQTT_PINGER_QOS 0
#define CONFIG_MQTT_PINGER_RETAINED 1
#define CONFIG_MQTT_PINGER_AS_PLAIN 0
#define CONFIG_MQTT_PINGER_AS_JSON 1
#endif // CONFIG_MQTT_PINGER_ENABLE

/*************** MQTT : remote control ***************/
// EN: Allow the device to process incoming commands
// RU: Разрешить обработку входящих команд устройством
#define CONFIG_MQTT_COMMAND_ENABLE 1
// EN: Allow OTA updates via a third party server
// RU: Разрешить OTA обновления через сторонний сервер
#define CONFIG_MQTT_OTA_ENABLE 1

/***************** MQTT : sensors ********************/
#define CONFIG_MQTT_MAX_OUTBOX_SIZE 0
// EN: Delay between update attempts
// RU: Интервал публикации данных с сенсоров в секундах
#define CONFIG_MQTT_SENSORS_SEND_INTERVAL 60
// EN: QOS for sensor data
// RU: QOS для данных с сенсоров
#define CONFIG_MQTT_SENSORS_QOS 1
#define CONFIG_MQTT_SENSORS_LOCAL_QOS 2
// EN: Save the last sent data on the broker
// RU: Сохранять на брокере последние отправленные данные
#define CONFIG_MQTT_SENSORS_RETAINED 1
#define CONFIG_MQTT_SENSORS_LOCAL_RETAINED 0

// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------- EN - http://open-monitoring.online/ --------------------------------------------
// -------------------------------------- RU - http://open-monitoring.online/ --------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// EN: Enable sending data to open-monitoring.online
// RU: Включить отправку данных на open-monitoring.online
#define CONFIG_OPENMON_ENABLE 0
#if CONFIG_OPENMON_ENABLE
// EN: Frequency of sending data in seconds
// RU: Периодичность отправки данных в секундах
#define CONFIG_OPENMON_SEND_INTERVAL 180
// EN: Controller ids and tokens for open-monitoring.online
// RU: Идентификаторы контроллеров и токены для open-monitoring.online
#define CONFIG_OPENMON_CTR01_ID 0
#define CONFIG_OPENMON_CTR01_TOKEN "-----"
// EN: Allow publication of ping results и системной информации on open-monitoring.online
// RU: Разрешить публикацию результатов пинга и системной информации на open-monitoring.online
#define CONFIG_OPENMON_PINGER_ENABLE 1
#if CONFIG_OPENMON_PINGER_ENABLE
#define CONFIG_OPENMON_PINGER_ID 1
#define CONFIG_OPENMON_PINGER_TOKEN "------"
#define CONFIG_OPENMON_PINGER_INTERVAL 180000
#define CONFIG_OPENMON_PINGER_RSSI 1
#define CONFIG_OPENMON_PINGER_HEAP_FREE 1
#define CONFIG_OPENMON_PINGER_HOSTS 0
#endif // CONFIG_OPENMON_PINGER_ENABLE
#endif // CONFIG_OPENMON_ENABLE

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------ EN - https://thingspeak.com/ -----------------------------------------------
// ------------------------------------------ RU - https://thingspeak.com/ -----------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// EN: Enable sending data to thingspeak.com
// RU: Включить отправку данных на thingspeak.com
#define CONFIG_THINGSPEAK_ENABLE 0
#if CONFIG_THINGSPEAK_ENABLE
// EN: Frequency of sending data in seconds
// RU: Периодичность отправки данных в секундах
#define CONFIG_THINGSPEAK_SEND_INTERVAL 300
// EN: Tokens for writing on thingspeak.com
// RU: Токены для записи на thingspeak.com
#define CONFIG_THINGSPEAK_CHANNEL01_ID 0
#define CONFIG_THINGSPEAK_CHANNEL01_KEY "--------------------"
#endif // CONFIG_THINGSPEAK_ENABLE

// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------- EN - Telegram notify ---------------------------------------------------
// ------------------------------------------- RU - Уведомления в Telegram -----------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// EN: Allow Telegram notifications (general flag)
// RU: Разрешить уведомления в Telegram (общий флаг)
#define CONFIG_TELEGRAM_ENABLE 1
#if CONFIG_TELEGRAM_ENABLE
// EN: Telegram API bot token
// RU: Токен бота API Telegram
#define CONFIG_TELEGRAM_TOKEN "8888888888:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA-VVVVVVV"
// EN: Chat or group ID
// RU: Идентификатор чата или группы
#define CONFIG_TELEGRAM_CHAT_ID_MAIN     "-1000000000001"
#define CONFIG_TELEGRAM_CHAT_ID_SERVICE  "-1000000000002"
#define CONFIG_TELEGRAM_CHAT_ID_PARAMS   CONFIG_TELEGRAM_CHAT_ID_SERVICE
// EN: Send message header (device name, see CONFIG_TELEGRAM_DEVICE)
// RU: Отправлять заголовок сообщения (название устройства, см. CONFIG_TELEGRAM_DEVICE)
#define CONFIG_TELEGRAM_TITLE_ENABLED 0
// EN: Device name (published at the beginning of each message)
// RU: Название устройства (публикуется в начале каждого сообщения)
#define CONFIG_TELEGRAM_DEVICE "ТЕСТ"
#endif // CONFIG_TELEGRAM_ENABLE

// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------- EN - Notifies ----------------------------------------------------------
// --------------------------------------------- RU - Уведомления --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// EN: Blink the system LED when sending a data
// RU: Мигать системным светодиодом при отправке данных
#define CONFIG_SYSLED_MQTT_ACTIVITY 1
#define CONFIG_SYSLED_SEND_ACTIVITY 1
#define CONFIG_SYSLED_TELEGRAM_ACTIVITY 1
// EN: Allow remote enabling or disabling of notifications without flashing the device
// RU: Разрешить удаленную включение или отключение уведомлений без перепрошивки устройства
#define CONFIG_NOTIFY_TELEGRAM_CUSTOMIZABLE 1

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------ EN - Electricity tariffs ---------------------------------------------------
// ----------------------------------------- RU - Тарифы электроэнергии --------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

// EN: Use switching between multiple electricity tariffs
// RU: Использовать переключение между несколькими тарифами электроэнергии
#define CONFIG_ELTARIFFS_ENABLED 0
#if defined(CONFIG_ELTARIFFS_ENABLED) && CONFIG_ELTARIFFS_ENABLED
// EN: Maximum number of tariffs
// RU: Максимальное количество тарифов
#define CONFIG_ELTARIFFS_COUNT 2
// EN: Type of values for tariff parameters (OPT_KIND_PARAMETER - only for this device, OPT_KIND_PARAMETER_LOCATION - for all location devices)
// RU: Тип занчений для параметров тарифов (OPT_KIND_PARAMETER - только для этого устройства, OPT_KIND_PARAMETER_LOCATION - для всех устройств локации)
#define CONFIG_ELTARIFFS_PARAMS_TYPE OPT_KIND_PARAMETER_LOCATION
// EN: Tariff parameter values: days of the week, time intervals and cost of 1 kW/h
// RU: Значения параметров тарифов: дни недели, интервалы времени и стоимость 1кВт/ч
#define CONFIG_ELTARIFFS_TARIF1_DAYS WEEK_ANY
#define CONFIG_ELTARIFFS_TARIF1_TIMESPAN 7002300UL
#define CONFIG_ELTARIFFS_TARIF1_PRICE 5.78
#define CONFIG_ELTARIFFS_TARIF2_DAYS WEEK_ANY
#define CONFIG_ELTARIFFS_TARIF2_TIMESPAN 23000700UL
#define CONFIG_ELTARIFFS_TARIF2_PRICE 3.02
#endif // CONFIG_ELTARIFFS_ENABLED

// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------- EN - Log -----------------------------------------------------------
// ------------------------------------------------ RU - Отладка ---------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// EN: Debug message level. Anything above this level will be excluded from the code
// RU: Уровень отладочных сообщений. Всё, что выше этого уровня, будет исключено из кода
#define CONFIG_RLOG_PROJECT_LEVEL RLOG_LEVEL_DEBUG
// EN: Add text color codes to messages. Doesn't work on Win7
// RU: Добавлять коды цвета текста к сообщениям. Не работает на Win7
#define CONFIG_RLOG_PROJECT_COLORS 1
// EN: Add time stamp to messages
// RU: Добавлять отметку времени к сообщениям
#define CONFIG_RLOG_SHOW_TIMESTAMP 1
// EN: Add file and line information to messages
// RU: Добавлять информацию о файле и строке к сообщениям
#define CONFIG_RLOG_SHOW_FILEINFO 0

// EN: Preserve debugging information across device software restarts
// RU: Сохранять отладочную информацию при программном перезапуске устройства
#define CONFIG_RESTART_DEBUG_INFO 1
// EN: Allow heap information to be saved periodically, with subsequent output on restart
// RU: Разрешить периодическое сохранение информации о куче / памяти с последующем выводом при перезапуске
#define CONFIG_RESTART_DEBUG_HEAP_SIZE_SCHEDULE 1
// EN: Depth to save the processor stack on restart. 0 - do not save
// RU: Глубина сохранения стека процессора при перезапуске. 0 - не сохранять
#define CONFIG_RESTART_DEBUG_STACK_DEPTH 28
// EN: Allow publishing debug info from WiFi module
// RU: Разрешить публикацию отладочной информации из модуля WiFi
#define CONFIG_WIFI_DEBUG_ENABLE 1
// EN: Allow periodic publication of system information
// RU: Разрешить периодическую публикацию системной информации
#define CONFIG_MQTT_SYSINFO_ENABLE 1
#define CONFIG_MQTT_SYSINFO_INTERVAL 60000           
#define CONFIG_MQTT_SYSINFO_SYSTEM_FLAGS 1
// EN: Allow periodic publication of task information. CONFIG_FREERTOS_USE_TRACE_FACILITY / configUSE_TRACE_FACILITY must be allowed
// RU: Разрешить периодическую публикацию информации о задачах. Должен быть разрешен CONFIG_FREERTOS_USE_TRACE_FACILITY / configUSE_TRACE_FACILITY
#define CONFIG_MQTT_TASKLIST_ENABLE 1
#define CONFIG_MQTT_TASKLIST_INTERVAL 300000           

// EN: Restart device on memory allocation errors
// RU: Перезапустить устройство при ошибках выделения памяти
#define CONFIG_HEAP_ALLOC_FAILED_RESTART 0
#define CONFIG_HEAP_ALLOC_FAILED_RETRY 0
#define CONFIG_HEAP_ALLOC_FAILED_RESTART_DELAY 300000
