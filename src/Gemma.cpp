#include <Arduino.h>
#include "HMIM_HMIManager.h"
#include "MW_Uploader.h"
#include "DefaultConfig.h"
#include "BLEHandler.h"
#include "MW_Strip.h"
#include "ConfigurationManager.h"
#include "PCF85063A.h"




void setup()
{
  Serial.begin(115200);

  // Initialize Configuration Manager
  ConfigurationManager configManager;
  configManager.initialize();


  // Initialize RTC
  PCF85063A rtc;
  rtc.initialize();



  pinMode(ROTARY_ENCODER_BUTTON_PIN, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);

  digitalWrite(PIN_LED, LOW);

  // Strip initialization
  MWST_Initialize();

  // Check if wifi update has to be started
  if (touchRead(PIN_CENTER_TS) < CAPTOUCH_THLD_BOOT || (digitalRead(ROTARY_ENCODER_BUTTON_PIN) == LOW))
  {
    MWST_ToggleStripState(STRIP_CENTER);
    MWST_SetBrightness(STRIP_CENTER, 100);
    MWST_SetStripColor(STRIP_CENTER, RgbwColor(0, 0x30, 0x10, 0));

    MWST_SetStripColor(STRIP_CENTER, RgbwColor(0x30, 0, 0x30));
    MWUP_EnterBootloaderMode();
  }

#ifndef BT_DEBUG
  // Initialize BLE
  BLEHandler_Initialize();
#else
  SerialBT.enableSSP();
  SerialBT.begin("GEMMA_DBG");
#endif

  // HMI Interface
  HMIM_Initialize();
}

void loop()
{
  Serial.print(rtc.getDay()); Serial.print(".");
  Serial.print(rtc.getMonth()); Serial.print(".");
  Serial.print(rtc.getYear()); Serial.print(". ");
  Serial.print(rtc.getHour()); Serial.print(":");
  Serial.print(rtc.getMinute()); Serial.print(":");
  Serial.println(rtc.getSecond());
  HMIN_ProcessHMI();
}
