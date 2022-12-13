
#include "HMIM_HMIManager.h"
#include "MW_Uploader.h"
#include "DefaultConfig.h"
#include "BLEHandler.h"
#include "MW_Strip.h"
#include "ConfigurationManager.h"
#include "PF85063A.h"
 PCF85063A rtc;


void setup()
{
  Serial.begin(115200);


  ConfigurationManager configManager;
  configManager.initialize();

  // Initialize RTC
 
  rtc.setTime(10, 30, 0);
  rtc.setDate(2, 13, 12, 2022);
  rtc.setAlarm(0,31,10,99,99);  



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
