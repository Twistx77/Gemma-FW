#include "BLEHandler.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
#include "MW_Strip.h"
#include <stdint.h>
#include "DefaultConfig.h"
#include "ConfigurationManager.h"

enum UUID
{
  SWITCH_SERVICE_UUID = 0,
  PARAMETERS_SERVICE_UUID = 1,
  DEVICE_INFO_SERVICE_UUID = 2,

  SWITCH_UUID = 3,
  BRIGHTNESS_UUID = 4,
  COLOR_UUID = 5,
  SWITCH_LEFT_UUID = 6,
  BRIGHTNESS_LEFT_UUID = 7,
  COLOR_LEFT_UUID = 8,
  SWITCH_RIGHT_UUID = 9,
  BRIGHTNESS_RIGHT_UUID = 10,
  COLOR_RIGHT_UUID = 11,

  PARAM_DEBUG_OUTPUT_UUID = 12,
  PARAM_PIN_STRIP_UUID = 13,
  PARAM_PIN_CENTER_TS_UUID = 14,
  PARAM_PIN_LEFT_TS_UUID = 15,
  PARAM_PIN_RIGHT_TS_UUID = 16,
  PARAM_PIN_LED_UUID = 17,
  PARAM_ROTARY_ENCODER_A_PIN_UUID = 18,
  PARAM_ROTARY_ENCODER_B_PIN_UUID = 19,
  PARAM_ROTARY_ENCODER_BUTTON_PIN_UUID = 20,
  PARAM_ROTARY_ENCODER_STEPS_UUID = 21,
  PARAM_ROTARY_ENCODER_ACCELERATION_UUID = 22,
  PARAM_NUMBER_OF_LEDS_UUID = 23,
  PARAM_NUMBER_OF_NL_LEDS_UUID = 24,
  PARAM_MAX_BRIGHTNESS_UUID = 25,
  PARAM_CAPTOUCH_THLD_BOOT_UUID = 26,

  FW_VERSION_UUID = 27,
  HW_VERSION_UUID = 28
};

const char *UUID_STRINGS[] = {
    "667d724e-4540-4123-984f-9ad6082212bb",
    "4b698caa-abfa-4f8b-b136-42590f64652e",
    "052699e8-1a9b-40fb-a14b-00b0772187d9",

    "14cdad1f-1b15-41ee-9f51-d5caaf940d01",
    "14cdad1f-1b15-41ee-9f51-d5caaf940d02",
    "14cdad1f-1b15-41ee-9f51-d5caaf940d03",
    "14cdad1f-1b15-41ee-9f51-d5caaf940d04",
    "14cdad1f-1b15-41ee-9f51-d5caaf940d05",
    "14cdad1f-1b15-41ee-9f51-d5caaf940d06",
    "14cdad1f-1b15-41ee-9f51-d5caaf940d07",
    "14cdad1f-1b15-41ee-9f51-d5caaf940d08",
    "14cdad1f-1b15-41ee-9f51-d5caaf940d09",

    "f0e9cb41-1b2b-4799-ab36-0ddb25e70900",
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70901",
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70902",
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70903",
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70904",
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70905",
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70906",
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70907",
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70908",
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70909",
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70910",
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70911",
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70912",
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70913",
    "f0e9cb41-1b2b-4799-ab36-0ddb25e70914",

    "99704284-4d6b-4812-a599-cfd570230c47",
    "4b88d539-a706-426e-885c-69bb0c04fa84"};

BLEServer *pServer;
BLEService *pSwitchService, *pDeviceInfoService;
BLECharacteristic *pCharSwitch, *pCharBrightness, *pCharColor;
BLECharacteristic *pCharSwitchLeft, *pCharBrightnessLeft, *pCharColorLeft;
BLECharacteristic *pCharSwitchRight, *pCharBrightnessRight, *pCharColorRight;
BLECharacteristic *pCharParamDebugOutput, *pCharParamPinStrip, *pCharParamPinCenterTS, *pCharParamPinLeftTS,
    *pCharParamPinRightTS, *pCharParamPinLED, *pCharParamRotaryEncoderAPin, *pCharParamRotaryEncoderBPin,
    *pCharParamRotaryEncoderButtonPin, *pCharParamRotaryEncoderSteps, *pCharParamRotaryEncoderAcceleration,
    *pCharParamNumberOfLEDs, *pCharParamNumberOfNLLEDs, *pCharParamMaxBrightness, *pCharParamCapTouchThldBoot;
BLECharacteristic *pCharFWVersion, *pCharHWVersion;

ConfigurationManager configManager;

bool deviceConnected = false;
float hue = 0;

class callbackSwitch : public BLECharacteristicCallbacks
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
    Serial.println("onRead switch");
    uint32_t state = 0;
    if (MWST_GetState(stripType))
      state = 1;
    pCharacteristic->setValue(state);

  } // onRead
};

class callbackBrightness : public BLECharacteristicCallbacks
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

class callbackColor : public BLECharacteristicCallbacks
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

    /*if (value.length() > 0)
    {
      Serial.print("Value:");
      // Serial.println(value.c_str());

      // Convert value to int
      int color = atoi(value.c_str());

      Serial.println(color);

      hue = map(color, 0, 512 - 255, 0, 60000) / 60000.0;

      if (color < (512 - 255))
      {

        MWST_SetStripColor(stripType, RgbwColor(HsbColor(hue, 0.8f, 1.0f)));
      }
      else
      {

        MWST_SetStripColor(stripType, RgbwColor(0, 0, map(512 - color, 0, 512 - 255, 0, 255), 255));
      };*/

    //}
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

class callbackParameters : public BLECharacteristicCallbacks
{

  void onWrite(BLECharacteristic *pCharacteristic)
  {
    // Get the parameter from the UUID last 2 characters converting them to a number by subtracting 30 (ASCII 0) and multiplying by 10 the first
    // character and adding the second character + PARAM_DEBUG_OUTPUT to add the offset of the parameters in the enum.
    ConfigParameter parameter = pCharacteristic->getUUID().getNative()->uuid.uuid128[1] - 30 * 10 + pCharacteristic->getUUID().getNative()->uuid.uuid128[0] - 30 + PARAM_DEBUG_OUTPUT;
    if (parameter < MAX_PARAMETERS && parameter >= PARAM_DEBUG_OUTPUT)
    {
      configManager.writeParameter(parameter, pCharacteristic->getData()[0]);
    }
  }
  void onRead(BLECharacteristic *pCharacteristic)
  {
    // Get the parameter from the UUID last 2 characters converting them to a number by subtracting 30 (ASCII 0) and multiplying by 10 the first
    // character and adding the second character + PARAM_DEBUG_OUTPUT to add the offset of the parameters in the enum.
    enum ConfigParameter offset = PARAM_DEBUG_OUTPUT;
    ConfigParameter parameter =(ConfigParameter) (offset + pCharacteristic->getUUID().getNative()->uuid.uuid128[1] - 30 * 10 + pCharacteristic->getUUID().getNative()->uuid.uuid128[0] - 30);// +( (uint8_t) offset));
    if (parameter < MAX_PARAMETERS && parameter >= PARAM_DEBUG_OUTPUT)
    {
      uint32_t value = configManager.readParameter(parameter);      
      pCharacteristic->setValue(value);
    }
  }
};

class callbackFWVersion : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic)
  {
    Serial.println("onRead FWVersion");
    // uint8_t version[] = {FW_MAJOR,FW_MINOR,FW_PATCH};
    // pCharacteristic->setValue(version,3);
  }
};

class callbackHWVersion : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic)
  {
    Serial.println("onRead HWVersion");
    pCharacteristic->setValue("0.2");
  }
};

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    Serial.println("Connected");
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
    Serial.println("Connected");
  }
};

void BLEHandler_Initialize()
{
  /*BLEDevice::init("Long name works now");
  pServer = BLEDevice::createServer();
  pSwitchService = pServer->createService(SERVICE_UUID);
  pSwitchService->start();
  pCharacteristic = pSwitchService->createCharacteristic(
                                         SWITCH_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setValue("Gemma Controller");
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();


  BLEDevice::init("ESP32-BLE-Server");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pSwitchService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pSwitchService->createCharacteristic(
                                         SWITCH_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Hello World");
  pSwitchService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();*/

  Serial.println("Initializing BLE");

  BLEDevice::init("Gemma Controller");

  BLEServer *pServer = BLEDevice::createServer();

  configManager = ConfigurationManager::getInstance();

  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pSwitchService = pServer->createService(UUID_STRINGS[SWITCH_SERVICE_UUID]);
  BLEService *pConfigurationService = pServer->createService(UUID_STRINGS[PARAMETERS_SERVICE_UUID]);
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

  BLECharacteristic *pCharFWVersion = pDeviceInfoService->createCharacteristic(
      UUID_STRINGS[FW_VERSION_UUID],
      BLECharacteristic::PROPERTY_READ);

  BLECharacteristic *pCharHWVersion = pDeviceInfoService->createCharacteristic(
      UUID_STRINGS[HW_VERSION_UUID],
      BLECharacteristic::PROPERTY_READ);

  pCharSwitch->setCallbacks(new callbackSwitch());
  pCharBrightness->setCallbacks(new callbackBrightness());
  pCharColor->setCallbacks(new callbackColor());
  pCharSwitchLeft->setCallbacks(new callbackSwitch());
  pCharBrightnessLeft->setCallbacks(new callbackBrightness());
  pCharColorLeft->setCallbacks(new callbackColor());
  pCharSwitchRight->setCallbacks(new callbackSwitch());
  pCharBrightnessRight->setCallbacks(new callbackBrightness());
  pCharColorRight->setCallbacks(new callbackColor());

  pCharParamDebugOutput->setCallbacks(new callbackParameters());
  pCharParamPinStrip->setCallbacks(new callbackParameters());
  pCharParamPinCenterTS->setCallbacks(new callbackParameters());
  pCharParamPinLeftTS->setCallbacks(new callbackParameters());
  pCharParamPinRightTS->setCallbacks(new callbackParameters());
  pCharParamPinLED->setCallbacks(new callbackParameters());
  pCharParamRotaryEncoderAPin->setCallbacks(new callbackParameters());
  pCharParamRotaryEncoderBPin->setCallbacks(new callbackParameters());
  pCharParamRotaryEncoderButtonPin->setCallbacks(new callbackParameters());
  pCharParamRotaryEncoderSteps->setCallbacks(new callbackParameters());
  pCharParamRotaryEncoderAcceleration->setCallbacks(new callbackParameters());
  pCharParamNumberOfLEDs->setCallbacks(new callbackParameters());
  pCharParamNumberOfNLLEDs->setCallbacks(new callbackParameters());
  pCharParamMaxBrightness->setCallbacks(new callbackParameters());
  pCharParamCapTouchThldBoot->setCallbacks(new callbackParameters());

  pCharFWVersion->setCallbacks(new callbackFWVersion());
  pCharHWVersion->setCallbacks(new callbackHWVersion());

  pSwitchService->start();
  pDeviceInfoService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();

  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);

  pAdvertising->start();
}