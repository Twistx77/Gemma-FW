
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
#include "../Core/ConfigurationManager.h"

enum UUID
{
  SWITCH_SERVICE_UUID = 0,
  PARAMETERS_SERVICE_UUID = 1,
  TIME_SERVICE_UUID = 2,
  DEVICE_INFO_SERVICE_UUID = 3,

  SWITCH_UUID = 4,
  BRIGHTNESS_UUID = 5,
  COLOR_UUID = 6,
  SWITCH_LEFT_UUID = 7,
  BRIGHTNESS_LEFT_UUID = 8,
  COLOR_LEFT_UUID = 9,
  SWITCH_RIGHT_UUID = 10,
  BRIGHTNESS_RIGHT_UUID = 11,
  COLOR_RIGHT_UUID = 12,

  PARAM_DEBUG_OUTPUT_UUID = 13,
  PARAM_PIN_STRIP_UUID = 14,
  PARAM_PIN_CENTER_TS_UUID = 15,
  PARAM_PIN_LEFT_TS_UUID = 16,
  PARAM_PIN_RIGHT_TS_UUID = 17,
  PARAM_PIN_LED_UUID = 18,
  PARAM_ROTARY_ENCODER_A_PIN_UUID = 19,
  PARAM_ROTARY_ENCODER_B_PIN_UUID = 20,
  PARAM_ROTARY_ENCODER_BUTTON_PIN_UUID = 21,
  PARAM_ROTARY_ENCODER_STEPS_UUID = 22,
  PARAM_ROTARY_ENCODER_ACCELERATION_UUID = 23,
  PARAM_NUMBER_OF_LEDS_UUID = 24,
  PARAM_NUMBER_OF_NL_LEDS_UUID = 25,
  PARAM_MAX_BRIGHTNESS_UUID = 26,
  PARAM_CAPTOUCH_THLD_BOOT_UUID = 27,

  SET_TIME_UUID = 28,
  SET_ALARM_TIME_UUID = 29,
  SET_ALARM_STATE_UUID = 30,

  FW_VERSION_UUID = 31,
  HW_VERSION_UUID = 32
};

const char *UUID_STRINGS[] = {
    "667d724e-4540-4123-984f-9ad6082212ba", // SWITCH_SERVICE_UUID
    "4b698caa-abfa-4f8b-b136-42590f64652e", // PARAMETERS_SERVICE_UUID
    "052699e8-1a9b-40fb-a14b-00b0772187d9", // TIME_SERVICE_UUID
    "21ec2541-a87d-45f6-a5d8-27aa9f742502", // DEVICE_INFO_SERVICE_UUID

    "14cdad1f-1b15-41ee-9f51-d5caaf940d01", // SWITCH_UUID
    "14cdad1f-1b15-41ee-9f51-d5caaf940d02", // BRIGHTNESS_UUID
    "14cdad1f-1b15-41ee-9f51-d5caaf940d03", // COLOR_UUID
    "14cdad1f-1b15-41ee-9f51-d5caaf940d04", // SWITCH_LEFT_UUID
    "14cdad1f-1b15-41ee-9f51-d5caaf940d05", // BRIGHTNESS_LEFT_UUID
    "14cdad1f-1b15-41ee-9f51-d5caaf940d06", // COLOR_LEFT_UUID
    "14cdad1f-1b15-41ee-9f51-d5caaf940d07", // SWITCH_RIGHT_UUID
    "14cdad1f-1b15-41ee-9f51-d5caaf940d08", // BRIGHTNESS_RIGHT_UUID
    "14cdad1f-1b15-41ee-9f51-d5caaf940d09", // COLOR_RIGHT_UUID

    "f0e9cb41-1b2b-4799-ab36-0ddb25e70900", // PARAM_DEBUG_OUTPUT_UUID
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70901", // PARAM_PIN_STRIP_UUID
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70902", // PARAM_PIN_CENTER_TS_UUID
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70903", // PARAM_PIN_LEFT_TS_UUID
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70904", // PARAM_PIN_RIGHT_TS_UUID
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70905", // PARAM_PIN_LED_UUID
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70906", // PARAM_ROTARY_ENCODER_A_PIN_UUID
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70907", // PARAM_ROTARY_ENCODER_B_PIN_UUID
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70908", // PARAM_ROTARY_ENCODER_BUTTON_PIN_UUID
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70909", // PARAM_ROTARY_ENCODER_STEPS_UUID
    "f0e9cb41-1b2b-4799-ab36-0ddb25e7090a", // PARAM_ROTARY_ENCODER_ACCELERATION_UUID
    "f0e9cb41-1b2b-4799-ab36-0ddb25e7090b", // PARAM_NUMBER_OF_LEDS_UUID
    "f0e9cb41-1b2b-4799-ab36-0ddb25e7090c", // PARAM_NUMBER_OF_NL_LEDS_UUID
    "f0e9cb41-1b2b-4799-ab36-0ddb25e7090d", // PARAM_MAX_BRIGHTNESS_UUID
    "f0e9cb41-1b2b-4799-ab36-0ddb25e7090e", // PARAM_CAPTOUCH_THLD_BOOT_UUID

    "21ec2541-a87d-45f6-a5d8-27aa9f742501", // SET_TIME_UUID
    "21ec2541-a87d-45f6-a5d8-27aa9f742502", // SET_ALARM_TIME_UUID
    "21ec2541-a87d-45f6-a5d8-27aa9f742503", // SET_ALARM_STATE_UUID

    "99704284-4d6b-4812-a599-cfd570230c47", // FW_VERSION_UUID
    "4b88d539-a706-426e-885c-69bb0c04fa84"  // HW_VERSION_UUID
};

ConfigurationManager configManager;
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
    ConfigParameter parameter = (ConfigParameter)(pCharacteristic->getUUID().getNative()->uuid.uuid128[0] + PARAM_DEBUG_OUTPUT);
    if (parameter < MAX_PARAMETERS && parameter >= PARAM_DEBUG_OUTPUT)
    {
      configManager.writeParameter(parameter, pCharacteristic->getData()[0]);
    }
  }
  void onRead(BLECharacteristic *pCharacteristic)
  {
    ConfigParameter parameter = (ConfigParameter)(pCharacteristic->getUUID().getNative()->uuid.uuid128[0] + PARAM_DEBUG_OUTPUT);
    if (parameter < MAX_PARAMETERS && parameter >= PARAM_DEBUG_OUTPUT)
    {
      uint32_t value = configManager.readParameter(parameter);

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
    uint8_t fwVersion[] = {configManager.readParameter(PARAM_FW_MAJOR), configManager.readParameter(PARAM_FW_MINOR), configManager.readParameter(PARAM_FW_PATCH)};
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

  configManager = ConfigurationManager::getInstance();
  configManager.initialize();

  alarmsManager.initialize();

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
      UUID_STRINGS[PARAM_CAPTOUCH_THLD_BOOT],
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

void checkBLE()
{
  if (alarmsManager.checkAlarms())
  {
    Serial.println("Alarm triggered");
  }
}