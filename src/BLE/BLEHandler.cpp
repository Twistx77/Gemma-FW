
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

const enum ServiceUUID
{
  DEVICE_INFO_SERVICE_UUID = "21ec2541-a87d-45f6-a5d8-27aa9f742502",
  CONTROL_SERVICE_UUID = "667d724e-4540-4123-984f-9ad6082212ba",
  ALARM_SERVICE_UUID = "052699e8-1a9b-40fb-a14b-00b0772187d9",
  PARAMETERS_SERVICE_UUID = "4b698caa-abfa-4f8b-b136-42590f64652e",
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


// Charactersitics array
const Characteristic[] =
{
  { "99704284-4d6b-4812-a599-cfd570230c47", DEVICE_INFO_SERVICE_UUID, BLECharacteristic::PROPERTY_READ}, // FW Version 
  { "4b88d539-a706-426e-885c-69bb0c04fa84", DEVICE_INFO_SERVICE_UUID, BLECharacteristic::PROPERTY_READ}, // HW Version

  { "14cdad1f-1b15-41ee-9f51-d5caaf940d01", CONTROL_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Swtich Center    "14cdad1f-1b15-41ee-9f51-d5caaf940d02", // BRIGHTNESS_UUID
  { "14cdad1f-1b15-41ee-9f51-d5caaf940d02", CONTROL_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Brightness Center
  { "14cdad1f-1b15-41ee-9f51-d5caaf940d03", CONTROL_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Color Center
  { "14cdad1f-1b15-41ee-9f51-d5caaf940d04", CONTROL_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Switch Left
  { "14cdad1f-1b15-41ee-9f51-d5caaf940d05", CONTROL_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Brightness Left
  { "14cdad1f-1b15-41ee-9f51-d5caaf940d06", CONTROL_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Color Left
  { "14cdad1f-1b15-41ee-9f51-d5caaf940d07", CONTROL_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Switch Right
  { "14cdad1f-1b15-41ee-9f51-d5caaf940d08", CONTROL_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Brightness Right
  { "14cdad1f-1b15-41ee-9f51-d5caaf940d09", CONTROL_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Color Right
  { "14cdad1f-1b15-41ee-9f51-d5caaf940d0a", CONTROL_SERVICE_UUID, BLECharacteristic::PROPERTY_WRITE }, // Reset Device

  { "21ec2541-a87d-45f6-a5d8-27aa9f742501", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Current Time
  { "21ec2541-a87d-45f6-a5d8-27aa9f742502", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 1 On Time
  { "21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 1 Off Time
  { "21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 1 Brightness
  { "21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 1 Color
  { "21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 1 Brightness Delay
  { "21ec2541-a87d-45f6-a5d8-27aa9f742502", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 2 On Time
  { "21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 2 Off Time
  { "21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 2 Brightness
  { "21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 2 Color
  { "21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 2 Brightness Delay
  { "21ec2541-a87d-45f6-a5d8-27aa9f742502", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 3 On Time
  { "21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 3 Off Time
  { "21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 3 Brightness
  { "21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 3 Color
  { "21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 3 Brightness Delay
  { "21ec2541-a87d-45f6-a5d8-27aa9f742502", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 4 On Time
  { "21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 4 Off Time
  { "21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 4 Brightness
  { "21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 4 Color
  { "21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 4 Brightness Delay
  { "21ec2541-a87d-45f6-a5d8-27aa9f742502", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 5 On Time
  { "21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 5 Off Time
  { "21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 5 Brightness
  { "21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 5 Color
  { "21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Alarm 5 Brightness Delay

   
  { "f0e9cb41-1b2b-4799-ab36-0ddb25e70900", PARAMETERS_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Number of LEDs Night Light Left
  { "f0e9cb41-1b2b-4799-ab36-0ddb25e70901", PARAMETERS_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Number of LEDs Night Light Right
  { "f0e9cb41-1b2b-4799-ab36-0ddb25e70900", PARAMETERS_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Hue value for Rotary Encoder

  {"2f7980c8-28d0-4c1c-ad2c-78036e8faf00", SPECIAL_COMMANDS_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Go to Bootloader
  {"2f7980c8-28d0-4c1c-ad2c-78036e8faf01", SPECIAL_COMMANDS_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Number of LEDs in Strip
  {"2f7980c8-28d0-4c1c-ad2c-78036e8faf02", SPECIAL_COMMANDS_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Captouch Threshold
  {"2f7980c8-28d0-4c1c-ad2c-78036e8faf03", SPECIAL_COMMANDS_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Current value of touch center
  {"2f7980c8-28d0-4c1c-ad2c-78036e8faf04", SPECIAL_COMMANDS_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Current value of touch left 
  {"2f7980c8-28d0-4c1c-ad2c-78036e8faf05", SPECIAL_COMMANDS_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Current value of touch right 
  {"2f7980c8-28d0-4c1c-ad2c-78036e8faf06", SPECIAL_COMMANDS_SERVICE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}, // Encoder
  
};


ConfigManager configManager = ConfigManager::getInstance();
AlarmsManager alarmsManager;

bool deviceConnected = false;
float hue = 0;

class CallbackSwitch : public BLECharacteristicCallbacks
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

class CallbackFWVersion : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic)
  {
    uint8_t fwVersion[] = {(uint8_t) configManager.getParameter(DefaultConfigParameters[ID_FW_MAJOR]), (uint8_t)configManager.getParameter(DefaultConfigParameters[ID_FW_MINOR]), (uint8_t)configManager.getParameter(DefaultConfigParameters[ID_FW_PATCH])};
    pCharacteristic->setValue(fwVersion, 3);
  }
};

class CallbackHWVersion : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic)
  {
    pCharacteristic->setValue("0.2");
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

  ConfigManager configManager = ConfigManager::getInstance();  s

  BLEService *pSwitchService = pServer->createService(UUID_STRINGS[SWITCH_SERVICE_UUID]);
  BLEService *pConfigurationService = pServer->createService(BLEUUID(UUID_STRINGS[PARAMETERS_SERVICE_UUID]), 40, 0); // 40 is the maximum number of handles  numHandles = (# of Characteristics)*2  +  (# of Services) + (# of Characteristics with BLE2902)
  // BLEService *pTimeService = pServer->createService(UUID_STRINGS[TIME_SERVICE_UUID]);
  BLEService *pDeviceInfoService = pServer->createService(UUID_STRINGS[DEVICE_INFO_SERVICE_UUID]);

  BLECharacteristic *pCharSwitch = pSwitchService->createCharacteristic(
      UUID_STRINGS[SWITCH_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharBrightness = pSwitchService->createCharacteristic(
      UUID_STRINGS[BRIGHTNESS_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharColor = pSwitchService->createCharacteristic(
      UUID_STRINGS[COLOR_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharSwitchLeft = pSwitchService->createCharacteristic(
      UUID_STRINGS[SWITCH_LEFT_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharBrightnessLeft = pSwitchService->createCharacteristic(
      UUID_STRINGS[BRIGHTNESS_LEFT_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharColorLeft = pSwitchService->createCharacteristic(
      UUID_STRINGS[COLOR_LEFT_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharSwitchRight = pSwitchService->createCharacteristic(
      UUID_STRINGS[SWITCH_RIGHT_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharBrightnessRight = pSwitchService->createCharacteristic(
      UUID_STRINGS[BRIGHTNESS_RIGHT_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharColorRight = pSwitchService->createCharacteristic(
      UUID_STRINGS[COLOR_RIGHT_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharParamDebugOutput = pConfigurationService->createCharacteristic(
      UUID_STRINGS[PARAM_DEBUG_OUTPUT_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharParamPinStrip = pConfigurationService->createCharacteristic(
      UUID_STRINGS[PARAM_PIN_STRIP_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharParamPinCenterTS = pConfigurationService->createCharacteristic(
      UUID_STRINGS[PARAM_PIN_CENTER_TS_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharParamPinLeftTS = pConfigurationService->createCharacteristic(
      UUID_STRINGS[PARAM_PIN_LEFT_TS_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharParamPinRightTS = pConfigurationService->createCharacteristic(
      UUID_STRINGS[PARAM_PIN_RIGHT_TS_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharParamPinLED = pConfigurationService->createCharacteristic(
      UUID_STRINGS[PARAM_PIN_LED_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharParamRotaryEncoderAPin = pConfigurationService->createCharacteristic(
      UUID_STRINGS[PARAM_ROTARY_ENCODER_A_PIN_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharParamRotaryEncoderBPin = pConfigurationService->createCharacteristic(
      UUID_STRINGS[PARAM_ROTARY_ENCODER_B_PIN_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharParamRotaryEncoderButtonPin = pConfigurationService->createCharacteristic(
      UUID_STRINGS[PARAM_ROTARY_ENCODER_BUTTON_PIN_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharParamRotaryEncoderSteps = pConfigurationService->createCharacteristic(
      UUID_STRINGS[PARAM_ROTARY_ENCODER_STEPS_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharParamRotaryEncoderAcceleration = pConfigurationService->createCharacteristic(
      UUID_STRINGS[PARAM_ROTARY_ENCODER_ACCELERATION_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharParamNumberOfLEDs = pConfigurationService->createCharacteristic(
      UUID_STRINGS[PARAM_NUMBER_OF_LEDS_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharParamNumberOfNLLEDs = pConfigurationService->createCharacteristic(
      UUID_STRINGS[PARAM_NUMBER_OF_NL_LEDS_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharParamMaxBrightness = pConfigurationService->createCharacteristic(
      UUID_STRINGS[PARAM_MAX_BRIGHTNESS_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharParamCapTouchThldBoot = pConfigurationService->createCharacteristic(
      UUID_STRINGS[PARAM_CAPTOUCH_THLD_BOOT_UUID],
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);
  /*
      BLECharacteristic *pCharCurrentTime = pTimeService->createCharacteristic(
          UUID_STRINGS[SET_TIME_UUID],
          BLECharacteristic::PROPERTY_READ |
              BLECharacteristic::PROPERTY_WRITE);

      BLECharacteristic *pCharAlarmState = pTimeService->createCharacteristic(
          UUID_STRINGS[SET_ALARM_TIME_UUID],
          BLECharacteristic::PROPERTY_READ |
              BLECharacteristic::PROPERTY_WRITE);

      BLECharacteristic *pCharAlarm = pTimeService->createCharacteristic(
          UUID_STRINGS[SET_ALARM_STATE_UUID],
          BLECharacteristic::PROPERTY_READ |
              BLECharacteristic::PROPERTY_WRITE);
              */

  BLECharacteristic *pCharFWVersion = pDeviceInfoService->createCharacteristic(
      UUID_STRINGS[FW_VERSION_UUID],
      BLECharacteristic::PROPERTY_READ);

  BLECharacteristic *pCharHWVersion = pDeviceInfoService->createCharacteristic(
      UUID_STRINGS[HW_VERSION_UUID],
      BLECharacteristic::PROPERTY_READ);

  pCharSwitch->setCallbacks(new CallbackSwitch());
  pCharBrightness->setCallbacks(new CallbackBrightness());
  pCharColor->setCallbacks(new CallbackColor());
  pCharSwitchLeft->setCallbacks(new CallbackSwitch());
  pCharBrightnessLeft->setCallbacks(new CallbackBrightness());
  pCharColorLeft->setCallbacks(new CallbackColor());
  pCharSwitchRight->setCallbacks(new CallbackSwitch());
  pCharBrightnessRight->setCallbacks(new CallbackBrightness());
  pCharColorRight->setCallbacks(new CallbackColor());

  pCharParamDebugOutput->setCallbacks(new CallbackParameters());
  pCharParamPinStrip->setCallbacks(new CallbackParameters());
  pCharParamPinCenterTS->setCallbacks(new CallbackParameters());
  pCharParamPinLeftTS->setCallbacks(new CallbackParameters());
  pCharParamPinRightTS->setCallbacks(new CallbackParameters());
  pCharParamPinLED->setCallbacks(new CallbackParameters());
  pCharParamRotaryEncoderAPin->setCallbacks(new CallbackParameters());
  pCharParamRotaryEncoderBPin->setCallbacks(new CallbackParameters());
  pCharParamRotaryEncoderButtonPin->setCallbacks(new CallbackParameters());
  pCharParamRotaryEncoderSteps->setCallbacks(new CallbackParameters());
  pCharParamRotaryEncoderAcceleration->setCallbacks(new CallbackParameters());
  pCharParamNumberOfLEDs->setCallbacks(new CallbackParameters());
  pCharParamNumberOfNLLEDs->setCallbacks(new CallbackParameters());
  pCharParamMaxBrightness->setCallbacks(new CallbackParameters());
  pCharParamCapTouchThldBoot->setCallbacks(new CallbackParameters());

  pCharFWVersion->setCallbacks(new CallbackFWVersion());
  pCharHWVersion->setCallbacks(new CallbackHWVersion());

  pSwitchService->start();
  pDeviceInfoService->start();
  pConfigurationService->start();
  // pTimeService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(pSwitchService->getUUID());
  pAdvertising->addServiceUUID(pDeviceInfoService->getUUID());
  pAdvertising->addServiceUUID(pConfigurationService->getUUID());
  // pAdvertising->addServiceUUID(pTimeService->getUUID());

  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  // BLEDevice::startAdvertising();
  pAdvertising->start();
}