
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <stdint.h>

#include "BLEHandler.h"
#include "../DefaultConfig.h"
#include "../Core/MW_Strip.h"
#include "../Core/MW_Uploader.h"
#include "../Core/AlarmsManager.h"
#include "../Configuration/ConfigManager.h"
#include "../Configuration/ConfigParameters.h"

typedef const char *const UUID_t;

#define DEVICE_INFO_SERVICE_UUID "21ec2541-a87d-45f6-a5d8-27aa9f742502"
#define CONTROL_SERVICE_UUID "667d724e-4540-4123-984f-9ad6082212ba"
#define ALARM_SERVICE_UUID "052699e8-1a9b-40fb-a14b-00b0772187d9"
#define CONFIGURATION_SERVICE_UUID "4b698caa-abfa-4f8b-b136-42590f64652e"
#define SERVICE_COMMANDS_UUID "12095a43-bcc4-4988-8d45-d2afcad7cd28"

// Structure to store the configuration parameter information
typedef struct Characteristic
{
  // UUID of the characteristic
  UUID_t UUID;
  // Service UUID
  UUID_t service;
  // Properties of the characteristic
  const uint32_t properties;

} Characteristic;

#define READ_ONLY BLECharacteristic::PROPERTY_READ
#define WRITE_ONLY BLECharacteristic::PROPERTY_WRITE
#define READ_WRITE READ_ONLY | WRITE_ONLY

// Info Service Charactersitics array
const Characteristic DeviceInfoCharacteristics[] =
    {
        {"99704284-4d6b-4812-a599-cfd570230c01", DEVICE_INFO_SERVICE_UUID, READ_ONLY}, // FW Version
        {"99704284-4d6b-4812-a599-cfd570230c02", DEVICE_INFO_SERVICE_UUID, READ_ONLY}, // HW Version

};

// Control Service Charactersitics array
const Characteristic ControlCharacteristics[]{
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d01", CONTROL_SERVICE_UUID, READ_WRITE}, // Swtich Center
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d02", CONTROL_SERVICE_UUID, READ_WRITE}, // Switch Left
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d03", CONTROL_SERVICE_UUID, READ_WRITE}, // Switch Right
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d04", CONTROL_SERVICE_UUID, READ_WRITE}, // Color Center
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d05", CONTROL_SERVICE_UUID, READ_WRITE}, // Color Left
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d06", CONTROL_SERVICE_UUID, READ_WRITE}, // Color Right
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d07", CONTROL_SERVICE_UUID, READ_WRITE}, // Brightness Center
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d08", CONTROL_SERVICE_UUID, READ_WRITE}, // Brightness Left
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d09", CONTROL_SERVICE_UUID, READ_WRITE}, // Brightness Right
};

// Alarm Service Charactersitics array

// Set Time Characteristic format
// Seconds[0], Minutes[1], Hours[2], Day[3], Month[4], Year[5], Weekday[6]

// Alarm Characteristic format
// Enabled[0]
// Time ON: Minutes[1], Hours[2], Weekdays[3]
// Time OFF: Minutes[4], Hours[5], Weekdays[6]
// Max Brightness: Brightness[7]
// Delay Max Brightness: SecondsMSB[8] SecondsLSB[9]
// Color: Red[10], Green[11], Blue[12], White[13]
const Characteristic AlarmCharacteristics[]{
    {"21ec2541-a87d-45f6-a5d8-27aa9f742501", ALARM_SERVICE_UUID, READ_WRITE}, // Current Time
    {"21ec2541-a87d-45f6-a5d8-27aa9f742502", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 1
    {"21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 2
    {"21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 3
    {"21ec2541-a87d-45f6-a5d8-27aa9f742505", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 4
    {"21ec2541-a87d-45f6-a5d8-27aa9f742506", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 5
    {"21ec2541-a87d-45f6-a5d8-27aa9f742507", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 6
    {"21ec2541-a87d-45f6-a5d8-27aa9f742508", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 7
    {"21ec2541-a87d-45f6-a5d8-27aa9f742509", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 8
    {"21ec2541-a87d-45f6-a5d8-27aa9f742510", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 9
    {"21ec2541-a87d-45f6-a5d8-27aa9f742511", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 10
};

// Parameter Service Characteristics
const Characteristic ConfigurationCharacteristics[]{
    {"f0e9cb41-1b2b-4799-ab36-0ddb25e70901", CONFIGURATION_SERVICE_UUID, READ_WRITE}, // Number of LEDs Night Light Left
    {"f0e9cb41-1b2b-4799-ab36-0ddb25e70902", CONFIGURATION_SERVICE_UUID, READ_WRITE}, // Number of LEDs Night Light Right
    {"f0e9cb41-1b2b-4799-ab36-0ddb25e70903", CONFIGURATION_SERVICE_UUID, READ_WRITE}, // Hue value for Rotary Encoder
};

// Service Commands Characteristics
const Characteristic ServiceCommandsCharacteristics[]{
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf01", SERVICE_COMMANDS_UUID, WRITE_ONLY}, // Reset Device
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf02", SERVICE_COMMANDS_UUID, WRITE_ONLY}, // Go to Bootloader
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf03", SERVICE_COMMANDS_UUID, WRITE_ONLY}, // Reset to Factory Defaults

    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf10", SERVICE_COMMANDS_UUID, READ_WRITE}, // Number of LEDs in Strip
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf11", SERVICE_COMMANDS_UUID, READ_WRITE}, // Captouch Threshold
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf12", SERVICE_COMMANDS_UUID, READ_WRITE}, // Encoder Resolution
};

ConfigManager configManager = ConfigManager::getInstance();
AlarmsManager alarmsManager;

bool deviceConnected = false;

class CallbackDeviceInfo : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic)
  {

    switch (pCharacteristic->getUUID().toString()[35])
    {
    case '1':
    {
      pCharacteristic->setValue(FW_VERSION);
    }
    break;

    case '2':
      pCharacteristic->setValue("1");
      break;

    default:
      break;
    }
  }
};

class CallbackControl : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    uint8_t stripType = pCharacteristic->getUUID().toString()[35] - '1'; // TRIP_CENTER = 0 ->  '1' - '1' = 0

    // Color
    if (stripType > 2 & stripType <= 5)
      stripType -= 3;
    // Brightness
    else if (stripType > 5 & stripType <= 8)
      stripType -= 6;

    uint8_t *state = pCharacteristic->getData();

    MWST_SetStripState(stripType, (state[0] == 1) ? MWST_ENABLED : MWST_DISABLED, EFFECT_FADE);

  } // onWrite

  void onRead(BLECharacteristic *pCharacteristic)
  {
    uint8_t stripType = pCharacteristic->getUUID().toString()[35] - '1'; // TRIP_CENTER = 0 ->  '1' - '1' = 0

    // Color
    if (stripType > 2 & stripType <= 5)
      stripType -= 3;
    // Brightness
    else if (stripType > 5 & stripType <= 8)
      stripType -= 6;

    uint8_t value[1];

    if (MWST_GetState(stripType) == MWST_ENABLED)
    {

      value[0] = {1};
    }
    else
    {
      value[0] = {0};
    }

    pCharacteristic->setValue(value, 1);

  } // onRead
};

class CallbackAlarm : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic)
  {
    uint8_t uUIDLastChar = pCharacteristic->getUUID().toString()[35];

    if (uUIDLastChar == '1')
    {
      // Get the curernt time from the AlarmsManager
      TimeAndDate timeAndDate = alarmsManager.getTimeAndDate();
      uint8_t timeAndDateArray[] = {timeAndDate.year, timeAndDate.month, timeAndDate.day, timeAndDate.hours, timeAndDate.minutes, timeAndDate.seconds};
      pCharacteristic->setValue(timeAndDateArray, 6);
    }
    else
    {
      uint8_t alarmInstance = uUIDLastChar - '1'; // Alarm 1 = 1 ->  '2' - '1' = 1
      // Get the alarm time from the AlarmsManager
      AlarmParameters alarmParameters = alarmsManager.getAlarm((Alarm)alarmInstance);

      uint8_t alarm[] = {(uint8_t)(alarmParameters.color >> 24 & 0xFF), (uint8_t)(alarmParameters.color >> 16 & 0xFF), (uint8_t)(alarmParameters.color >> 8 & 0xFF), (uint8_t)(alarmParameters.color & 0xFF),
                         (uint8_t)(alarmParameters.secondsToFullBrightness >> 8 & 0xFF), (uint8_t)(alarmParameters.secondsToFullBrightness & 0xFF), alarmParameters.maxBrightness,
                         alarmParameters.timeAndDateOff.weekday, alarmParameters.timeAndDateOff.hours, alarmParameters.timeAndDateOff.minutes,
                         alarmParameters.timeAndDateOn.weekday, alarmParameters.timeAndDateOn.hours, alarmParameters.timeAndDateOn.minutes, alarmParameters.enabled};

      pCharacteristic->setValue(alarm, 14);
    }
  }

  void onWrite(BLECharacteristic *pCharacteristic)
  {
    uint8_t uUIDLastChar = pCharacteristic->getUUID().toString()[35];

    if (uUIDLastChar == '1')
    {
      uint8_t *currentTime = pCharacteristic->getData();

      TimeAndDate timeAndDate;
      timeAndDate.seconds = currentTime[0];
      timeAndDate.minutes = currentTime[1];
      timeAndDate.hours = currentTime[2];
      timeAndDate.day = currentTime[3];
      timeAndDate.month = currentTime[4];
      timeAndDate.year = currentTime[5];
      timeAndDate.weekday = currentTime[6];
      alarmsManager.setTimeAndDate(timeAndDate);
    }
    else
    {
      uint8_t *newAlarmParameters = pCharacteristic->getData();
      AlarmParameters alarmParameters;

      alarmParameters.color = (newAlarmParameters[13] << 24) | (newAlarmParameters[12] << 16) | (newAlarmParameters[11] << 8) | newAlarmParameters[10];
      alarmParameters.secondsToFullBrightness = (newAlarmParameters[9] << 8) | newAlarmParameters[8];
      alarmParameters.maxBrightness = newAlarmParameters[7];
      alarmParameters.timeAndDateOff.weekday = newAlarmParameters[6];
      alarmParameters.timeAndDateOff.hours = newAlarmParameters[5];
      alarmParameters.timeAndDateOff.minutes = newAlarmParameters[4];
      alarmParameters.timeAndDateOn.weekday = newAlarmParameters[3];
      alarmParameters.timeAndDateOn.hours = newAlarmParameters[2];
      alarmParameters.timeAndDateOn.minutes = newAlarmParameters[1];
      alarmParameters.enabled = newAlarmParameters[0];
      alarmsManager.setAlarm(Alarm(uUIDLastChar - '1'), alarmParameters);
    }
  }
};

class CallbackConfiguration : public BLECharacteristicCallbacks
{

  void onWrite(BLECharacteristic *pCharacteristic)
  {
    ParameterID parameter = (ParameterID)(pCharacteristic->getUUID().toString()[35] - '0' + ID_HW_VERSION); // UUIDLastChar - 30 = 1

    if (parameter < ID_HW_VERSION && parameter >= MAX_CONFIG_PARAMETERS)
    {
      configManager.setParameter(DefaultParametersConfig[parameter], pCharacteristic->getData()[0]);
    }
  }
  void onRead(BLECharacteristic *pCharacteristic)
  {
    ParameterID parameter = (ParameterID)(pCharacteristic->getUUID().toString()[35] - '0' + ID_HW_VERSION); // UUIDLastChar - 30 = 1
    if (parameter < ID_HW_VERSION && parameter >= MAX_CONFIG_PARAMETERS)
    {
      uint32_t value = configManager.getParameter(DefaultParametersConfig[parameter]);
      pCharacteristic->setValue(value);
    }
  }
};

class CallbackServiceCommands : public BLECharacteristicCallbacks
{

  void onWrite(BLECharacteristic *pCharacteristic)
  {
    uint8_t uUIDLastChar = pCharacteristic->getUUID().toString()[35] - '0' + ID_HW_VERSION; // UUIDLastChar - 30
    switch (uUIDLastChar)
    {
    case 1: // '01' Reset device
      ESP.restart();
      break;

    case 2: // '02' Go to Bootloader
      MWST_ToggleStripState(STRIP_CENTER);
      MWST_SetBrightness(STRIP_CENTER, 100);
      MWST_SetStripColor(STRIP_CENTER, RgbwColor(0x30, 0, 0x30));
      MWUP_EnterBootloaderMode();
      break;

    case 3: // '03' Reset Parameters to default
      configManager.resetToDefault();
      break;

    case 16: // '10' Set number of LEDS in strip
    case 17: // '11' Captouch threshold
    case 18: // '12' Encoder Resolution
    {
      uint8_t parameterID = uUIDLastChar - 16 + ID_LEDS_STRIP;
      configManager.setParameter(DefaultParametersConfig[uUIDLastChar], pCharacteristic->getData()[0]);
    }
    break;

      break;

    default:
      break;
    }
  }
  void onRead(BLECharacteristic *pCharacteristic)
  {
    uint8_t uUIDLastChar = pCharacteristic->getUUID().toString()[35] - '0' + ID_HW_VERSION; // UUIDLastChar - 30
    switch (uUIDLastChar)
    {
    case 16: // '10' Set number of LEDS in strip
    case 17: // '11' Captouch threshold
    case 18: // '12' Encoder Resolution
    {
      uint8_t parameterID = uUIDLastChar - 16 + ID_LEDS_STRIP;
      uint32_t value = configManager.getParameter(DefaultParametersConfig[uUIDLastChar]);
      pCharacteristic->setValue(value);
    }
    break;
    }
  }
};

class BLEConnectionsCallback : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
    BLEDevice::startAdvertising();
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
    BLEDevice::startAdvertising();
  }
};

void BLEHandler_Initialize()
{

  // Get the uinoque chip ID to use as the name of the BLE device
  uint32_t chipId;
  for (uint8_t i = 0; i < 17; i = i + 8)
  {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  String BLEName = "Gemma " + String(chipId, HEX);

  BLEDevice::init(std::string(BLEName.c_str()));

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BLEConnectionsCallback());

  ConfigManager configManager = ConfigManager::getInstance();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();

  // Initialize Device Info Service and Characteristics
  BLEService *pDeviceInfoService = pServer->createService(DEVICE_INFO_SERVICE_UUID);
  pAdvertising->addServiceUUID(pDeviceInfoService->getUUID());
  for (uint8_t i = 0; i < sizeof(DeviceInfoCharacteristics) / sizeof(Characteristic); i++)
  {
    BLECharacteristic *pCharacteristic = pDeviceInfoService->createCharacteristic(
        DeviceInfoCharacteristics[i].UUID,
        DeviceInfoCharacteristics[i].properties);
    pCharacteristic->setCallbacks(new CallbackDeviceInfo());
  }
  pDeviceInfoService->start();

  BLEService *pControlService = pServer->createService(CONTROL_SERVICE_UUID);
  pAdvertising->addServiceUUID(pControlService->getUUID());
  for (uint8_t i = 0; i < sizeof(ControlCharacteristics) / sizeof(Characteristic); i++)
  {
    BLECharacteristic *pCharacteristic = pControlService->createCharacteristic(
        ControlCharacteristics[i].UUID,
        ControlCharacteristics[i].properties);
    pCharacteristic->setCallbacks(new CallbackControl());
  }
  pControlService->start();

  BLEUUID alarmServiceUUID(ALARM_SERVICE_UUID);
  BLEService *pAlarmService = pServer->createService(alarmServiceUUID, 40, 0); // 40 is the maximum number of handles  numHandles = (# of Characteristics)*2  +  (# of Services) + (# of Characteristics with BLE2902)
  pAdvertising->addServiceUUID(pAlarmService->getUUID());
  for (uint8_t i = 0; i < sizeof(AlarmCharacteristics) / sizeof(Characteristic); i++)
  {
    BLECharacteristic *pCharacteristic = pAlarmService->createCharacteristic(
        AlarmCharacteristics[i].UUID,
        AlarmCharacteristics[i].properties);
    pCharacteristic->setCallbacks(new CallbackAlarm());
  }
  pAlarmService->start();

  BLEUUID configurationServiceUUID(CONFIGURATION_SERVICE_UUID);
  BLEService *pConfigurationService = pServer->createService(configurationServiceUUID, 40, 1);
  pAdvertising->addServiceUUID(pConfigurationService->getUUID());
  for (uint8_t i = 0; i < sizeof(ConfigurationCharacteristics) / sizeof(Characteristic); i++)
  {
    BLECharacteristic *pCharacteristic = pConfigurationService->createCharacteristic(
        ConfigurationCharacteristics[i].UUID,
        ConfigurationCharacteristics[i].properties);
    pCharacteristic->setCallbacks(new CallbackConfiguration());
  }
  pConfigurationService->start();

  BLEUUID serviceCommandsUUID(SERVICE_COMMANDS_UUID);
  BLEService *pServiceCommandsService = pServer->createService(serviceCommandsUUID, 40, 2);
  pAdvertising->addServiceUUID(pServiceCommandsService->getUUID());
  for (uint8_t i = 0; i < sizeof(ServiceCommandsCharacteristics) / sizeof(Characteristic); i++)
  {
    BLECharacteristic *pCharacteristic = pServiceCommandsService->createCharacteristic(
        ServiceCommandsCharacteristics[i].UUID,
        ServiceCommandsCharacteristics[i].properties);
    pCharacteristic->setCallbacks(new CallbackServiceCommands());
  }

  pServiceCommandsService->start();

  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);

  pAdvertising->start();
}