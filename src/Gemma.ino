
#include "HMIM_HMIManager.h"

#include "MW_Uploader.h"
#include "ConfigurationManager.h"
#include "DefaultConfig.h"
#include "BLEHandler.h"
#include "MW_Strip.h"

#include "PrettyDebug.h"




uint32_t NumberOfLedsStrip = MAX_NUMBER_OF_LEDS;

void setup()
{
  Serial.begin(115200);

  pinMode(ROTARY_ENCODER_BUTTON_PIN, INPUT_PULLUP);

  ATTACH_DEBUG_STREAM(&Serial);
  DEBUG_OK("Booting");
  
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
