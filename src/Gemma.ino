
#include "HMIM_HMIManager.h"
#include "MW_Uploader.h"
#include "ConfigurationManager.h"
#include "DefaultConfig.h"
#include "BLEHandler.h"
#include "MW_Strip.h"
#include "Preferences.h"

Preferences preferences;

void writeDefaultPreferences(void)
{
  preferences.putUShort(FW_MAJOR, DEFAULT_FW_MAJOR);
  preferences.putUShort(FW_MINOR, DEFAULT_FW_MINOR);
  preferences.putUShort(FW_PATCH, DEFAULT_FW_PATCH);
  preferences.putUShort(PIN_STRIP, DEFAULT_PIN_STRIP);

  preferences.putUShort(NUMBER_OF_LEDS, DEFAULT_NUMBER_OF_LEDS);
  preferences.putUShort(NUMBER_OF_NL_LEDS, DEFAULT_NUMBER_OF_LEDS_NL);
  preferences.putUShort(MAX_BRIGHTNESS, DEFAULT_MAX_BRIGHTNESS);
  preferences.putUShort(MAX_BRIGHTNESS_NL, DEFAULT_MAX_BRIGHTNESS_NL);

  preferences.putBool('EMPTY', false);


}

void initializePreference(void)
{
  preferences.begin("GLOBAL", false);

// Check if the preferences are empty . If it is (New chip) then set the default values
if (preferences.getBool('EMPTY'), true ){

}


}

void setup()
{
  Serial.begin(115200);

  pinMode(ROTARY_ENCODER_BUTTON_PIN, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);

  digitalWrite(PIN_LED, LOW);

  ATTACH_DEBUG_STREAM(&Serial);
  
  ConfigManager_Initialize();

  NumberOfLedsStrip = ConfigManager_ReadParameter(PARAM_NUMBER_OF_LEDS);

  // Strip initialization
  MWST_Initialize();
  
  // Check if wifi update has to be started
  if (touchRead(PIN_CENTER_TS) < CAPTOUCH_THLD_BOOT || (digitalRead(ROTARY_ENCODER_BUTTON_PIN)== LOW))
  {  
    MWST_ToggleStripState (STRIP_CENTER);
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
  DEBUG_OK("Initialization finished");
}

void loop()
{  
  HMIN_ProcessHMI();
}
