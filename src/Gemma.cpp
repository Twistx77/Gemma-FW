#include <Arduino.h>

#include "DefaultConfig.h"

#include "./Core/HMIM_HMIManager.h"
#include "./Core/MW_Uploader.h"
#include "./Core/MW_Strip.h"
#include "./Core/ConfigurationManager.h"

#include "./BLE/BLEHandler.h"

#include "./Drivers/PCF85063A.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

bool previousState = false;
// Initialize RTC
PCF85063A rtc;
void IRAM_ATTR rtc_int_isr() {
  previousState = !previousState;
}

void HMI_Task(void *arg)
{
    while(1){
        HMIM_ProcessHMI();
        vTaskDelay(10/ portTICK_RATE_MS);
    }
}

TaskHandle_t HMITaskHandle = NULL;
void setup()
{
  Serial.begin(115200);

  // Initialize Configuration Manager
  ConfigurationManager configManager;
  configManager.initialize();

  // Signage LED pin as output
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);



  pinMode(ROTARY_ENCODER_BUTTON_PIN, INPUT_PULLUP);

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

  xTaskCreate(HMI_Task, "HMI_Task", 10000, NULL, 1, &HMITaskHandle);
}

void loop()
{
  
}
