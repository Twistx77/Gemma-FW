#include "HMIM_HMIManager.h"
#include "MW_Strip.h"
#include "MW_Uploader.h"
#include <NeoPixelBrightnessBus.h>
#include "ConfigurationManager.h"
#include "DefaultConfig.h"
#include "BLEHandler.h"

#define CAPACITIVE_TOUCH_THRESHOLD 7

#define PIN_RIGHT_SENSOR 12
#define PIN_LEFT_SENSOR 13 //12 is the correct one for the rest of the devices



#define FW_UPDATE_TIME 5000


uint32_t NumberOfLedsStrip = MAX_NUMBER_OF_LEDS;

void setup()
{
 
  ConfigManager_Initialize();
  
  NumberOfLedsStrip = ConfigManager_ReadParameter(PARAM_NUMBER_OF_LEDS);

  Serial.begin(115200);
 
  
  MWST_Initialize();

  if (touchRead(PIN_LEFT_SENSOR) < CAPACITIVE_TOUCH_THRESHOLD)
  {
    while(touchRead(PIN_LEFT_SENSOR) < CAPACITIVE_TOUCH_THRESHOLD);  

      MWST_SetStripColor(STRIP_CENTER, RgbwColor(0xFF, 0, 0, 0));
      MWUP_EnterBootloaderMode();
    
  }
  BLEHandler_Initialize();
  HMIM_Initialize();

}

void loop()
{
  HMIN_ProcessHMI();
  
}


