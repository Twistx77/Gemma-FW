#include "MW_InputHandler.h"
#include "MW_StateMachine.h"
#include "MW_Strip.h"
#include "MW_Uploader.h"
#include <NeoPixelBrightnessBus.h>
#include "ConfigurationManager.h"




//#define DEBUG_MINIMUM trues
//#define DEBUG_INITIAL_LEVEL DEBUG_LEVEL_VERBOSE

#define POT_THRESHOLD 5
#define PIN_POT 35

#define CAPACITIVE_TOUCH_THRESHOLD 11

#define PIN_RIGHT_SENSOR 13
#define PIN_LEFT_SENSOR 12

#define LEDS_PER_STRIP 81

#define FW_UPDATE_TIME 5000
  float hue;
  float potValue;
  uint16_t lastPotValue;

void setup()
{
    ConfigManager_Initialize();
    //ConfigManager_Parameters().NumberOfLEDs
    Serial.begin(115200); 

    MWST_InitializeStrip(STRIP_LEFT, LEDS_PER_STRIP, 0, LEDS_PER_STRIP);

    //lastPotValue=analogRead(PIN_POT)
    if (touchRead(PIN_LEFT_SENSOR)<CAPACITIVE_TOUCH_THRESHOLD)
    {
      uint32_t startTime = millis();
      MWST_SetStripColor(STRIP_LEFT, RgbwColor(0xFF,0,0xFF,0));
      while(touchRead(PIN_LEFT_SENSOR)<CAPACITIVE_TOUCH_THRESHOLD & millis()-startTime<FW_UPDATE_TIME);
      if (millis()-startTime<FW_UPDATE_TIME < FW_UPDATE_TIME)
      {
          

      }
      MWST_SetStripColor(STRIP_LEFT, RgbwColor(0xFF,0,0,0));

      

      MWUP_EnterBootloaderMode();
    }

    
    MWIH_EnableInputSensor(MWIH_LEFT_SENSOR, PIN_LEFT_SENSOR);
    MWIH_EnableInputSensor(MWIH_RIGHT_SENSOR, PIN_RIGHT_SENSOR);
    MWSM_InitalizeStateMachine();
    
     


    
}

void loop()
{

    MWSM_RunStateMachine();
    //printlnD("Event: Left Sensor Released");
   //debugHandle();
   readPot();
}

void readPot()
{
    potValue = 0;
    for (int i=0; i<10; i++)
    {
    potValue +=analogRead(PIN_POT);
    }
    potValue = potValue/10;
   
    if (potValue < (lastPotValue - POT_THRESHOLD) || potValue > (lastPotValue + POT_THRESHOLD) )
    {
      lastPotValue = potValue;
    
      hue = map( potValue, 0, 4095-2048, 0, 60000)/60000.0;


    if (potValue<(4095-2048))
    {
      
    MWST_SetStripColor(STRIP_LEFT, RgbwColor(HsbColor(hue,0.8f,1.0f)));
    
   
    
    }
    else
    { 
      
      MWST_SetStripColor(STRIP_LEFT, RgbwColor(0,0,map( 4095-potValue, 0, 4095-2048, 0, 255),255));
    }
 
    }
  
}
