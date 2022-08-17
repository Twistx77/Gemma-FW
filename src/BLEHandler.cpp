#include "BLEHandler.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
#include "MW_Strip.h"
#include <NeoPixelBrightnessBus.h>


#define SERVICE_UUID "667d724e-4540-4123-984f-9ad6082212bb"
#define DEVICE_INFO_UUID "052699e8-1a9b-40fb-a14b-00b0772187d9"
#define SWITCH_UUID "14cdad1f-1b15-41ee-9f51-d5caaf940d01"
#define BRIGHTNESS_UUID "14cdad1f-1b15-41ee-9f51-d5caaf940d02"
#define COLOR_UUID "14cdad1f-1b15-41ee-9f51-d5caaf940d03"
#define SWITCH_LEFT_UUID "14cdad1f-1b15-41ee-9f51-d5caaf940d04"
#define BRIGHTNESS_LEFT_UUID "14cdad1f-1b15-41ee-9f51-d5caaf940d05"
#define COLOR_LEFT_UUID "14cdad1f-1b15-41ee-9f51-d5caaf940d06"
#define SWITCH_RIGHT_UUID "14cdad1f-1b15-41ee-9f51-d5caaf940d07"
#define BRIGHTNESS_RIGHT_UUID "14cdad1f-1b15-41ee-9f51-d5caaf940d08"
#define COLOR_RIGHT_UUID "14cdad1f-1b15-41ee-9f51-d5caaf940d09"
#define FW_VERSION_UUID "99704284-4d6b-4812-a599-cfd570230c47"
#define HW_VERSION_UUID "4b88d539-a706-426e-885c-69bb0c04fa84"

BLEServer *pServer;
BLEService *pSwitchService, *pDeviceInfoService;
BLECharacteristic *pCharSwitch, *pCharBrightness, *pCharColor;
BLECharacteristic *pCharSwitchLeft, *pCharBrightnessLeft, *pCharColorLeft;
BLECharacteristic *pCharSwitchRight, *pCharBrightnessRight, *pCharColorRight;
BLECharacteristic *pCharFWVersion, *pCharHWVersion;

bool deviceConnected = false;
float hue = 0;

class callbackSwitch : public BLECharacteristicCallbacks
{

  uint8_t stripType;
  switch (pchar->getUUID().toString()[35])
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

  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0)
    {
      Serial.println(value[0]);

      if (value[0] == '1')
      {
        MWST_SetStripState(stripType, MWST_ENABLED, EFFECT_PROGRESSIVE);
      }
      else if (value[0] == '0')
      {
        MWST_SetStripState(stripType, MWST_DISABLED, EFFECT_PROGRESSIVE);
      }
    }
  } // onWrite

  void onRead(BLECharacteristic *pCharacteristic)
  {
    Serial.println("onRead switch");
    bool state = MWST_GetState(stripType);
    pCharacteristic->setValue(state ? 1 : 0);

  } // onRead
};

class callbackBrightness : public BLECharacteristicCallbacks
{

  uint8_t stripType;
  switch (pchar->getUUID().toString()[35])
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
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0)
    {
      Serial.print("Value:");
      // Serial.println(value.c_str());

      // Convert value to int
      int brightness = atoi(value.c_str());
      Serial.println(brightness);
      MWST_SetBrightness(stripType, brightness);
    }
  }
  void onRead(BLECharacteristic *pCharacteristic)
  {
    Serial.println("onRead brightness");
    int brightness = MWST_GetBrightness(stripType);
    std::string value = pCharacteristic->setValue(String(brightness));
  }
};

class callbackColor : public BLECharacteristicCallbacks
{
  uint8_t stripType;
  switch (pchar->getUUID().toString()[35])
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
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0)
    {
      Serial.print("Value:");
      // Serial.println(value.c_str());

      // Convert value to int
      int color = atoi(value.c_str());

      Serial.println(color);

      hue = map(color, 0, 4095 - 1500, 0, 60000) / 60000.0;

      if (color < (4095 - 1500))
      {

        MWST_SetStripColor(stripType, RgbwColor(HsbColor(hue, 0.8f, 1.0f)));
      }
      else
      {

        MWST_SetStripColor(stripType, RgbwColor(0, 0, map(4095 - color, 0, 4095 - 2048, 0, 255), 255));
      };
    }
  } // onWrite
  
  void onRead(BLECharacteristic *pCharacteristic)
  {
    Serial.println("onRead color");
    int color = MWST_GetColorIndex(stripType);
    pCharacteristic->setValue(String(color));
  } // onRead
};

class callbackFWVersion : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic)
  {
    Serial.println("onRead FWVersion");
    pCharacteristic->setValue("1.0");
  }
};

class callbackHWVersion : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic)
  {
    Serial.println("onRead HWVersion");
    pCharacteristic->setValue("1.0");
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

  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pSwitchService = pServer->createService(SERVICE_UUID);
  BLEService *pDeviceInfoService = pServer->createService(DEVICE_INFO_UUID);

  BLECharacteristic *pCharSwitch = pSwitchService->createCharacteristic(
      SWITCH_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharBrightness = pSwitchService->createCharacteristic(
      BRIGHTNESS_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharColor = pSwitchService->createCharacteristic(
      COLOR_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharSwitchLeft = pSwitchService->createCharacteristic(
      SWITCH_LEFT_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharBrightnessLeft = pSwitchService->createCharacteristic(
      BRIGHTNESS_LEFT_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharColorLeft = pSwitchService->createCharacteristic(
      COLOR_LEFT_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharSwitchRight = pSwitchService->createCharacteristic(
      SWITCH_RIGHT_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharBrightnessRight = pSwitchService->createCharacteristic(
      BRIGHTNESS_RIGHT_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharColorRight = pSwitchService->createCharacteristic(
      COLOR_RIGHT_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharFWVersion = pSwitchService->createCharacteristic(
      FW_VERSION_UUID,
      BLECharacteristic::PROPERTY_READ);

  BLECharacteristic *pCharHWVersion = pSwitchService->createCharacteristic(
      HW_VERSION_UUID,
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

  pCharFWVersion->setCallbacks(new callbackFWVersion());
  pCharHWVersion->setCallbacks(new callbackHWVersion());

  pSwitchService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();

  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);

  pAdvertising->start();
}