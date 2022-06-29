#include "HMIM_HMIManager.h"
#include "MW_Strip.h"
#include "MW_Uploader.h"
#include <NeoPixelBrightnessBus.h>
#include "ConfigurationManager.h"
#include "DefaultConfig.h"
#include "BLEHandler.h"

#define CAPACITIVE_TOUCH_THRESHOLD 10

#define PIN_CENTER_TS 27 // TODO: REPLACE CONFIG MANAGER
#define PIN_LEFT_TS 2   // TODO: REPLACE CONFIG MANAGER
#define PIN_RIGHT_TS 4  // TODO: REPLACE CONFIG MANAGER



uint32_t NumberOfLedsStrip = MAX_NUMBER_OF_LEDS;

void setup()
{
 
  ConfigManager_Initialize();
  
  NumberOfLedsStrip = ConfigManager_ReadParameter(PARAM_NUMBER_OF_LEDS);

  Serial.begin(115200);


  MWST_Initialize();
  
   

  if (touchRead(PIN_LEFT_TS) < CAPACITIVE_TOUCH_THRESHOLD)
  {

    MWST_SetStripColor(STRIP_LEFT, RgbwColor(0, 0x30, 0x10, 0));
    while(touchRead(PIN_LEFT_TS) < CAPACITIVE_TOUCH_THRESHOLD)
    {
      Serial.println("Boot");
    }  

      MWST_SetStripColor(STRIP_LEFT, RgbwColor(0x30, 0, 0x30));
      MWUP_EnterBootloaderMode();
    
  }
  //BLEHandler_Initialize();
  HMIM_Initialize();

}

void loop()
{
  HMIN_ProcessHMI();
  
}


