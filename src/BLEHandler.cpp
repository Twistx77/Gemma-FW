#include "BLEHandler.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
#include "MW_Strip.h"
#include <NeoPixelBrightnessBus.h>

#define SERVICE_UUID "667d724e-4540-4123-984f-9ad6082212bb"
#define SWITCH_UUID "14cdad1f-1b15-41ee-9f51-d5caaf940d01"
#define BRIGHTNESS_UUID "14cdad1f-1b15-41ee-9f51-d5caaf940d02"
#define COLOR_UUID "14cdad1f-1b15-41ee-9f51-d5caaf940d03"

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharSwitch;
BLECharacteristic *pCharBrightness;
BLECharacteristic *pCharColor;

bool deviceConnected = false;
float hue = 0;

class callbackSwitch : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0)
    {
      Serial.println(value[0]);

      if (value[0] == '1')
      {
        MWST_SetStripState(STRIP_LEFT, MWST_ENABLED, EFFECT_PROGRESIVE);
      }
      else if (value[0] == '0')
      {
        MWST_SetStripState(STRIP_LEFT, MWST_DISABLED, EFFECT_PROGRESIVE);
      }
    } 
  } // onWrite
  void onRead(BLECharacteristic *pCharacteristic)
  {
    Serial.println("onRead switch");
    std::string value = pCharacteristic->setValue("1");
    
  } // onRead
};

class callbackBrightness : public BLECharacteristicCallbacks
{
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
      MWST_SetBrightness(STRIP_LEFT, brightness);
    }
  }
};

class callbackColor : public BLECharacteristicCallbacks
{
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

        MWST_SetStripColor(STRIP_LEFT, RgbwColor(HsbColor(hue, 0.8f, 1.0f)));
      }
      else
      {

        MWST_SetStripColor(STRIP_LEFT, RgbwColor(0, 0, map(4095 - color, 0, 4095 - 2048, 0, 255), 255));
      };
    }
  }
};

void onRead(BLECharacteristic *pchar)
{
  Serial.println("Read");
}
}
;

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
  pService = pServer->createService(SERVICE_UUID);
  pService->start();
  pCharacteristic = pService->createCharacteristic(
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

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         SWITCH_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Hello World");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();*/

  Serial.println("Initializing BLE");

  BLEDevice::init("Gemma Controller");

  BLEServer *pServer = BLEDevice::createServer();

  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharSwitch = pService->createCharacteristic(
      SWITCH_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharBrightness = pService->createCharacteristic(
      BRIGHTNESS_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pCharColor = pService->createCharacteristic(
      COLOR_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  pCharSwitch->setCallbacks(new callbackSwitch());
  pCharBrightness->setCallbacks(new callbackBrightness());
  pCharColor->setCallbacks(new callbackColor());

  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();

  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);

  pAdvertising->start();
}