
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
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d02", CONTROL_SERVICE_UUID, READ_WRITE}, // Brightness Center
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d03", CONTROL_SERVICE_UUID, READ_WRITE}, // Color Center
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d04", CONTROL_SERVICE_UUID, READ_WRITE}, // Switch Left
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d05", CONTROL_SERVICE_UUID, READ_WRITE}, // Brightness Left
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d06", CONTROL_SERVICE_UUID, READ_WRITE}, // Color Left
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d07", CONTROL_SERVICE_UUID, READ_WRITE}, // Switch Right
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d08", CONTROL_SERVICE_UUID, READ_WRITE}, // Brightness Right
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d09", CONTROL_SERVICE_UUID, READ_WRITE}, // Color Right
      {"14cdad1f-1b15-41ee-9f51-d5caaf940d0a", CONTROL_SERVICE_UUID, WRITE_ONLY}, // Reset Device
}

// Alarm Service Charactersitics array
const Characteristic AlarmCharacteristics[]
{
      {"21ec2541-a87d-45f6-a5d8-27aa9f742501", ALARM_SERVICE_UUID, READ_WRITE},     // Current Time
      {"21ec2541-a87d-45f6-a5d8-27aa9f742502", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 1 On Time
      {"21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 1 Off Time
      {"21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 1 Brightness
      {"21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 1 Color
      {"21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 1 Brightness Delay
      {"21ec2541-a87d-45f6-a5d8-27aa9f742502", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 2 On Time
      {"21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 2 Off Time
      {"21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 2 Brightness
      {"21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 2 Color
      {"21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 2 Brightness Delay
      {"21ec2541-a87d-45f6-a5d8-27aa9f742502", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 3 On Time
      {"21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 3 Off Time
      {"21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 3 Brightness
      {"21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 3 Color
      {"21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 3 Brightness Delay
      {"21ec2541-a87d-45f6-a5d8-27aa9f742502", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 4 On Time
      {"21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 4 Off Time
      {"21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 4 Brightness
      {"21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 4 Color
      {"21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 4 Brightness Delay
      {"21ec2541-a87d-45f6-a5d8-27aa9f742502", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 5 On Time
      {"21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 5 Off Time
      {"21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 5 Brightness
      {"21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 5 Color
      {"21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 5 Brightness Delay
}

// Parameter Service Characteristics
const Characteristic ConfigurationCharacteristics[]
{
      {"f0e9cb41-1b2b-4799-ab36-0ddb25e70901", CONFIGURATION_SERVICE_UUID, READ_WRITE},     // Number of LEDs Night Light Left
      {"f0e9cb41-1b2b-4799-ab36-0ddb25e70902", CONFIGURATION_SERVICE_UUID, READ_WRITE}, // Number of LEDs Night Light Right
      {"f0e9cb41-1b2b-4799-ab36-0ddb25e70903", CONFIGURATION_SERVICE_UUID, READ_WRITE}, // Hue value for Rotary Encoder
}

// Service Commands Characteristics
const Characteristic ServiceCommandsCharacteristics[]{
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf01", SPECIAL_COMMANDS_SERVICE_UUID, READ_WRITE}, // Go to Bootloader
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf02", SPECIAL_COMMANDS_SERVICE_UUID, READ_WRITE}, // Number of LEDs in Strip
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf03", SPECIAL_COMMANDS_SERVICE_UUID, READ_WRITE}, // Captouch Threshold
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf04", SPECIAL_COMMANDS_SERVICE_UUID, READ_WRITE}, // Current value of touch center
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf05", SPECIAL_COMMANDS_SERVICE_UUID, READ_WRITE}, // Current value of touch left
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf06", SPECIAL_COMMANDS_SERVICE_UUID, READ_WRITE}, // Current value of touch right
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf07", SPECIAL_COMMANDS_SERVICE_UUID, READ_WRITE}, // Encoder Resolution
}

ConfigManager configManager = ConfigManager::getInstance();
AlarmsManager alarmsManager;

bool deviceConnected = false;
float hue = 0;

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
    uint8_t stripType;
    switch (pCharacteristic->getUUID().toString()[35])
    {
    case '1':
      stripType = STRIP_CENTER;
      break;
    case '4':
      stripType = STRIP_LEFT;
      break;
    case '7':
      stripType = STRIP_RIGHT;
      break;
    }

    uint8_t *state = pCharacteristic->getData();
    if (state[0] == 1)
    {
      MWST_SetStripState(stripType, MWST_ENABLED, EFFECT_PROGRESSIVE);
    }
    else if (state[0] == 0)
    {
      MWST_SetStripState(stripType, MWST_DISABLED, EFFECT_PROGRESSIVE);
    }
  } // onWrite

  void onRead(BLECharacteristic *pCharacteristic)
  {
    uint8_t stripType;
    switch (pCharacteristic->getUUID().toString()[35])
    {
    case '1':
      stripType = STRIP_CENTER;
      break;
    case '4':
      stripType = STRIP_LEFT;
      break;
    case '7':
      stripType = STRIP_RIGHT;
      break;
    }
    uint32_t state = 0;
    if (MWST_GetState(stripType))
      state = 1;
    pCharacteristic->setValue(state);

  } // onRead
};

class CallbackBrightness : public BLECharacteristicCallbacks
{

  void onWrite(BLECharacteristic *pCharacteristic)
  {
    uint8_t stripType;
    switch (pCharacteristic->getUUID().toString()[35])
    {
    case '2':
      stripType = STRIP_CENTER;
      break;
    case '5':
      stripType = STRIP_LEFT;
      break;
    case '8':
      stripType = STRIP_RIGHT;
      break;
    }
    uint8_t *brightness = pCharacteristic->getData();
    MWST_SetBrightness(stripType, brightness[0]);
  }

  void onRead(BLECharacteristic *pCharacteristic)
  {
    uint8_t stripType;
    switch (pCharacteristic->getUUID().toString()[35])
    {
    case '2':
      stripType = STRIP_CENTER;
      break;
    case '5':
      stripType = STRIP_LEFT;
      break;
    case '8':
      stripType = STRIP_RIGHT;
      break;
    }
    int brightness = MWST_GetCurrentBrightness(stripType);
    pCharacteristic->setValue(brightness);
  }
};

class CallbackColor : public BLECharacteristicCallbacks
{
  uint8_t stripType;
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    switch (pCharacteristic->getUUID().toString()[35])
    {
    case '3':
      stripType = STRIP_CENTER;
      break;
    case '6':
      stripType = STRIP_LEFT;
      break;
    case '9':
      stripType = STRIP_RIGHT;
      break;
    }
    uint8_t *rgbwValue = pCharacteristic->getData();

    MWST_SetStripColor(stripType, RgbwColor(rgbwValue[1], rgbwValue[0], rgbwValue[2], rgbwValue[3]));

  } // onWrite

  void onRead(BLECharacteristic *pCharacteristic)
  {
    switch (pCharacteristic->getUUID().toString()[35])
    {
    case '3':
      stripType = STRIP_CENTER;
      break;
    case '6':
      stripType = STRIP_LEFT;
      break;
    case '9':
      stripType = STRIP_RIGHT;
      break;
    }
    RgbwColor color = MWST_GetColor(stripType);
    uint8_t rgbwValue[] = {color.R, color.G, color.B, color.W};
    pCharacteristic->setValue(rgbwValue, 4);
  } // onRead
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

class CallbackTime : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic)
  {

    switch (pCharacteristic->getUUID().toString()[35])
    {
    case '1':
    {

      // Get the curernt time from the AlarmsManager
      TimeAndDate timeAndDate = alarmsManager.getTimeAndDate();
      uint8_t timeAndDateArray[] = {timeAndDate.year, timeAndDate.month, timeAndDate.day, timeAndDate.hours, timeAndDate.minutes, timeAndDate.seconds};
      pCharacteristic->setValue(timeAndDateArray, 6);
    }
    break;

    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    {
      // Get the alarm time from the AlarmsManager
      AlarmParameters alarmParameters = alarmsManager.getAlarm(Alarm(pCharacteristic->getUUID().toString()[35] - '2'));

      uint8_t alarm[] = {alarmParameters.weekdays, alarmParameters.hours, alarmParameters.minutes, alarmParameters.enabled};
      pCharacteristic->setValue(alarm, 4);
    }
    break;

    default:
      break;
    }
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