#include <Arduino.h>

#include "DefaultConfig.h"

#include "./Core/MW_Strip.h"
#include "./Core/MW_Uploader.h"
#include "./Core/AlarmsManager.h"
#include "./Core/HMIM_HMIManager.h"

#include "./Configuration/ConfigManager.h"

#include "./BLE/BLEHandler.h"

#include "./Drivers/PCF85063A.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

AlarmsManager alarmsManager;

TaskHandle_t HMITaskHandle = NULL;
TaskHandle_t AlarmsTaskHandle = NULL;

// HMI Task
void HMI_Task(void *arg)
{
    while(1){
        HMIM_ProcessHMI();
        vTaskDelay(TASK_DELAY_HMI_MS/ portTICK_RATE_MS);
    }
}

// Alarms Task 
void AlarmsTask(void *arg)
{
    while(1){
        alarmsManager.checkAlarms();
        vTaskDelay(TASK_DELAY_ALARMS_MS/ portTICK_RATE_MS);
    }
}


void setup()
{
    // Signage LED pin as output
  pinMode(PIN_LED_DEFAULT, OUTPUT);
  digitalWrite(PIN_LED_DEFAULT, HIGH);

  Serial.begin(115200);

  // Initialize Configuration Manager
  ConfigManager configManager = ConfigManager::getInstance();
  configManager.initialize();

  // Set encoder button as input pull up to detect bootloader mode activation if the button is pressed
  pinMode(ROTARY_ENCODER_BUTTON_PIN, INPUT_PULLUP);

  // Strip initialization
  MWST_Initialize();

  uint32_t bootloader_enable = configManager.getParameter(DefaultParametersConfig[ID_BOOTLOADER_ENABLED]);

  // Check if wifi update has to be started
  if (touchRead(PIN_CENTER_TS_DEFAULT) < CAPTOUCH_THLD_BOOT || (digitalRead(ROTARY_ENCODER_BUTTON_PIN) == LOW) || (bootloader_enable == 1))
  {
    MWST_ToggleStripState(STRIP_CENTER);
    MWST_SetBrightness(STRIP_CENTER, 100);

    MWST_SetStripColor(STRIP_CENTER, RgbwColor(0x30, 0, 0x30));
    MWUP_EnterBootloaderMode();
  }
  
  // Initialize Alarms Manager
  alarmsManager.initialize();  

#ifndef BT_DEBUG
  // Initialize BLE
  BLEHandler_Initialize(&alarmsManager);
#else
  SerialBT.enableSSP();
  SerialBT.begin("GEMMA_DBG");
#endif

  // HMI Interface
  HMIM_Initialize();

  xTaskCreate(HMI_Task, "HMI_Task", 10000, NULL, 1, &HMITaskHandle);
  xTaskCreate(AlarmsTask, "AlarmsTask", 10000, NULL, 1, &AlarmsTaskHandle);
  
  // Turn off Signage LED to indicate that the device is ready
  digitalWrite(PIN_LED_DEFAULT, LOW);
}

void loop()
{
  // Idle
}
