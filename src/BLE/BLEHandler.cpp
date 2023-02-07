
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

const enum ServiceUUID {
  DEVICE_INFO_SERVICE_UUID = "21ec2541-a87d-45f6-a5d8-27aa9f742502",
  CONTROL_SERVICE_UUID = "667d724e-4540-4123-984f-9ad6082212ba",
  ALARM_SERVICE_UUID = "052699e8-1a9b-40fb-a14b-00b0772187d9",
  CONFIGURATION_SERVICE_UUID = "4b698caa-abfa-4f8b-b136-42590f64652e",
  SERVICE_COMMANDS_UUID = "12095a43-bcc4-4988-8d45-d2afcad7cd28"
};

// Structure to store the configuration parameter information
typedef struct Characteristic
{
  // UUID of the characteristic
  const char *const UUID;
  // Service UUID
  ServiceUUID service;
  // Properties of the characteristic
  const uint32_t properties;

} Characteristic;

#define READ_ONLY BLECharacteristic::PROPERTY_READ
#define WRITE_ONLY BLECharacteristic::PROPERTY_WRITE
#define READ_WRITE READ_WRITE

// Info Service Charactersitics array
const Characteristic DeviceInfoCharacteristics[] =
    {
        {"99704284-4d6b-4812-a599-cfd570230c47", DEVICE_INFO_SERVICE_UUID, READ_ONLY}, // FW Version
        {"4b88d539-a706-426e-885c-69bb0c04fa84", DEVICE_INFO_SERVICE_UUID, READ_ONLY}, // HW Version

}

// Control Service Charactersitics array
const Characteristic ControlCharacteristics[]
{
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d01", CONTROL_SERVICE_UUID, READ_WRITE},     // Swtich Center    "14cdad1f-1b15-41ee-9f51-d5caaf940d02", // BRIGHTNESS_UUID
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d02", CONTROL_SERVICE_UUID, READ_WRITE}, // Switch Left
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d03", CONTROL_SERVICE_UUID, READ_WRITE}, // Switch Right
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d04", CONTROL_SERVICE_UUID, READ_WRITE}, // Color Center
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d05", CONTROL_SERVICE_UUID, READ_WRITE}, // Color Left
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d06", CONTROL_SERVICE_UUID, READ_WRITE}, // Color Right
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d07", CONTROL_SERVICE_UUID, READ_WRITE}, // Brightness Center
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d08", CONTROL_SERVICE_UUID, READ_WRITE}, // Brightness Left
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d09", CONTROL_SERVICE_UUID, READ_WRITE}, // Brightness Right
}





// Alarm Service Charactersitics array

// Set Time Characteristic format
// Seconds[0], Minutes[1], Hours[2], Day[3], Month[4], Year[5], Weekday[6]

// Alarm Characteristic format
// Enabled[0]
// Time ON: Minutes[1], Hours[2], Weekdays[3]
// Time OFF: Minutes[4], Hours[5], Weekdays[6]
// Max Brightness: Brightness[7]
// Delay Max Brightness: Seconds[9] Minutes[8] 
// Color: Red[10], Green[11], Blue[12], White[13]
const Characteristic AlarmCharacteristics[]
{
      {"21ec2541-a87d-45f6-a5d8-27aa9f742501", ALARM_SERVICE_UUID, READ_WRITE},  // Current Time
      {"21ec2541-a87d-45f6-a5d8-27aa9f742502", ALARM_SERVICE_UUID, READ_WRITE},  // Alarm 1 
      {"21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, READ_WRITE},  // Alarm 2
      {"21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, READ_WRITE},  // Alarm 3
      {"21ec2541-a87d-45f6-a5d8-27aa9f742505", ALARM_SERVICE_UUID, READ_WRITE},  // Alarm 4
      {"21ec2541-a87d-45f6-a5d8-27aa9f742506", ALARM_SERVICE_UUID, READ_WRITE},  // Alarm 5
      {"21ec2541-a87d-45f6-a5d8-27aa9f742507", ALARM_SERVICE_UUID, READ_WRITE},  // Alarm 6
      {"21ec2541-a87d-45f6-a5d8-27aa9f742508", ALARM_SERVICE_UUID, READ_WRITE},  // Alarm 7
      {"21ec2541-a87d-45f6-a5d8-27aa9f742509", ALARM_SERVICE_UUID, READ_WRITE},  // Alarm 8
      {"21ec2541-a87d-45f6-a5d8-27aa9f7425010", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 9
      {"21ec2541-a87d-45f6-a5d8-27aa9f7425011", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 10
}

// Parameter Service Characteristics
const Characteristic ConfigurationCharacteristics[]
{
      {"f0e9cb41-1b2b-4799-ab36-0ddb25e70901", CONFIGURATION_SERVICE_UUID, READ_WRITE}, // Number of LEDs Night Light Left
      {"f0e9cb41-1b2b-4799-ab36-0ddb25e70902", CONFIGURATION_SERVICE_UUID, READ_WRITE}, // Number of LEDs Night Light Right
      {"f0e9cb41-1b2b-4799-ab36-0ddb25e70903", CONFIGURATION_SERVICE_UUID, READ_WRITE}, // Hue value for Rotary Encoder
}

// Service Commands Characteristics
const Characteristic ServiceCommandsCharacteristics[]{
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf01", SPECIAL_COMMANDS_SERVICE_UUID, WRITE_ONLY}, // Reset Device
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf02", SPECIAL_COMMANDS_SERVICE_UUID, READ_WRITE}, // Go to Bootloader
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf03", SPECIAL_COMMANDS_SERVICE_UUID, READ_WRITE}, // Number of LEDs in Strip
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf04", SPECIAL_COMMANDS_SERVICE_UUID, READ_WRITE}, // Captouch Threshold
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf05", SPECIAL_COMMANDS_SERVICE_UUID, READ_WRITE}, // Current value of touch center
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf06", SPECIAL_COMMANDS_SERVICE_UUID, READ_WRITE}, // Current value of touch left
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf07", SPECIAL_COMMANDS_SERVICE_UUID, READ_WRITE}, // Current value of touch right
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf08", SPECIAL_COMMANDS_SERVICE_UUID, READ_WRITE}, // Encoder Resolution
}

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
      uint8_t fwVersion[] = {(uint8_t)configManager.getParameter(DefaultConfigParameters[ID_FW_MAJOR]), (uint8_t)configManager.getParameter(DefaultConfigParameters[ID_FW_MINOR]), (uint8_t)configManager.getParameter(DefaultConfigParameters[ID_FW_PATCH])};
      pCharacteristic->setValue(fwVersion, 3);
      break;

    case '2':
      pCharacteristic->setValue("0.2");
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

    MWST_SetStripState(stripType, (state == 1) ? MWST_ENABLED : MWST_DISABLED, EFFECT_FADE);

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

    pCharacteristic->setValue((MWST_GetState(stripType) == MWST_ENABLED) ? 1 : 0);

  } // onRead
};

class CallbackAlarm : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic)
  {

    uint8_t uUUIDLastChar = pCharacteristic->getUUID().toString()[35];

    if (uUUIDLastChar == '1')
    {
      // Get the curernt time from the AlarmsManager
      TimeAndDate timeAndDate = alarmsManager.getTimeAndDate();
      uint8_t timeAndDateArray[] = {timeAndDate.year, timeAndDate.month, timeAndDate.day, timeAndDate.hours, timeAndDate.minutes, timeAndDate.seconds};
      pCharacteristic->setValue(timeAndDateArray, 6);
    }
    else if (uUUIDLastChar >= '2' && uUUIDLastChar <= '5')

      // Get the alarm time from the AlarmsManager
      AlarmParameters alarmParameters = alarmsManager.getAlarm(Alarm(pCharacteristic->getUUID().toString()[35] - '2'));

    uint8_t alarm[] = {alarmParameters.weekdays, alarmParameters.hours, alarmParameters.minutes, alarmParameters.enabled};
    pCharacteristic->setValue(alarm, 4);
  }

  void onWrite(BLECharacteristic *pCharacteristic)
  {
    switch (pCharacteristic->getUUID().toString()[35])
    {
    case '1':
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
    break;

    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    {
      uint8_t *currentAlarm = pCharacteristic->getData();
      AlarmParameters alarmParameters;
      alarmParameters.weekdays = currentAlarm[3];
      alarmParameters.hours = currentAlarm[2];
      alarmParameters.minutes = currentAlarm[1];
      alarmParameters.enabled = currentAlarm[0];
      alarmsManager.setAlarm(Alarm(pCharacteristic->getUUID().toString()[35] - '2'), alarmParameters);
    }
    break;

    default:
      break;
    }
  }
};

class CallbackParameters : public BLECharacteristicCallbacks
{

  void onWrite(BLECharacteristic *pCharacteristic)
  {
    ParameterID parameter = (ParameterID)(pCharacteristic->getUUID().getNative()->uuid.uuid128[0] + ID_DEBUG_OUT);
    if (parameter < MAX_CONFIG_PARAMETERS && parameter >= ID_DEBUG_OUT)
    {
      configManager.setParameter(DefaultConfigParameters[parameter], pCharacteristic->getData()[0]);
    }
  }
  void onRead(BLECharacteristic *pCharacteristic)
  {
    ParameterID parameter = (ParameterID)(pCharacteristic->getUUID().getNative()->uuid.uuid128[0] + ID_DEBUG_OUT);
    if (parameter < MAX_CONFIG_PARAMETERS && parameter >= ID_DEBUG_OUT)
    {
      uint32_t value = configManager.getParameter(DefaultConfigParameters[parameter]);
      pCharacteristic->setValue(value);
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
    BLECharacteristic *pCharacteristic = pAlarmService->createCharacteristic(
        DeviceInfoCharacteristics[i].uuid,
        DeviceInfoCharacteristics[i].properties);
    pCharacteristic->setCallbacks(new CallbackDeviceInfo());
  }
  pDeviceInfoService->start();

  BLEService *pControlService = pServer->createService(CONTROL_SERVICE_UUID)
                                    pAdvertising->addServiceUUID(pControlService->getUUID());
  for (uint8_t i = 0; i < sizeof(ControlCharacteristics) / sizeof(Characteristic); i++)
  {
    BLECharacteristic *pCharacteristic = pAlarmService->createCharacteristic(
        ControlCharacteristics[i].uuid,
        ControlCharacteristics[i].properties);
    pCharacteristic->setCallbacks(new CallbackControl());
  }
  pControlService->start();

  BLEService *pAlarmService = pServer->createService(ALARM_SERVICE_UUID, 40, 0); // 40 is the maximum number of handles  numHandles = (# of Characteristics)*2  +  (# of Services) + (# of Characteristics with BLE2902)
  pAdvertising->addServiceUUID(pAlarmService->getUUID());
  for (uint8_t i = 0; i < sizeof(AlarmCharacteristics) / sizeof(Characteristic); i++)
  {
    BLECharacteristic *pCharacteristic = pAlarmService->createCharacteristic(
        AlarmCharacteristics[i].uuid,
        AlarmCharacteristics[i].properties);
    pCharacteristic->setCallbacks(new CallbackAlarm());
  }
  pAlarmService->start();

  BLEService *pConfigurationService = pServer->createService(CONFIGURATION_SERVICE_UUID, 40, 0);
  pAdvertising->addServiceUUID(pConfigurationService->getUUID());
  for (uint8_t i = 0; i < sizeof(ConfigurationCharacteristics) / sizeof(Characteristic); i++)
  {
    BLECharacteristic *pCharacteristic = pAlarmService->createCharacteristic(
        ConfigurationCharacteristics[i].uuid,
        ConfigurationCharacteristics[i].properties);
    pCharacteristic->setCallbacks(new CallbackConfiguration());
  }
  pConfigurationService->start();

  BLEService *pServiceCommandsService = pServer->createService(SERVICE_COMMANDS_UUID, 40, 0);
  pAdvertising->addServiceUUID(pServiceCommandsService->getUUID());
  for (uint8_t i = 0; i < sizeof(ServiceCommandsCharacteristics) / sizeof(Characteristic); i++)
  {
    BLECharacteristic *pCharacteristic = pAlarmService->createCharacteristic(
        ServiceCommandsCharacteristics[i].uuid,
        ServiceCommandsCharacteristics[i].properties);
    pCharacteristic->setCallbacks(new CallbackServiceCommands());
  }
  pServiceCommandsService->start();

  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);

  pAdvertising->start();
}