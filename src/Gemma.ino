
#include "HMIM_HMIManager.h"

#include "MW_Uploader.h"
#include <NeoPixelBrightnessBus.h>
#include "ConfigurationManager.h"
#include "DefaultConfig.h"
#include "BLEHandler.h"
#include "MW_Strip.h"

#define CAPACITIVE_TOUCH_THRESHOLD 6

#define PIN_CENTER_TS 27 // TODO: REPLACE CONFIG MANAGER
#define PIN_LEFT_TS 2    // TODO: REPLACE CONFIG MANAGER
#define PIN_RIGHT_TS 4   // TODO: REPLACE CONFIG MANAGER



uint32_t NumberOfLedsStrip = MAX_NUMBER_OF_LEDS;

void setup()
{

  Serial.begin(115200);
  
  ConfigManager_Initialize();

  NumberOfLedsStrip = ConfigManager_ReadParameter(PARAM_NUMBER_OF_LEDS);

  
  MWST_Initialize();

  

  if (touchRead(PIN_CENTER_TS) < CAPACITIVE_TOUCH_THRESHOLD || touchRead(PIN_LEFT_TS) < CAPACITIVE_TOUCH_THRESHOLD )
  {
  

    MWST_ToggleStripState (STRIP_CENTER);
    MWST_SetBrightness(STRIP_CENTER, 100);
    MWST_SetStripColor(STRIP_CENTER, RgbwColor(0, 0x30, 0x10, 0));


    MWST_SetStripColor(STRIP_CENTER, RgbwColor(0x30, 0, 0x30));
    MWUP_EnterBootloaderMode();
  }
  BLEHandler_Initialize();
  HMIM_Initialize();
}

void loop()
{
  HMIN_ProcessHMI();
}
