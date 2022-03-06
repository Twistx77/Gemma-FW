#include "MW_InputHandler.h"
#include "MW_StateMachine.h"
#include "MW_Strip.h"
#include "MW_Uploader.h"
#include <NeoPixelBrightnessBus.h>
#include "ConfigurationManager.h"
#include "DefaultConfig.h"

//#define DEBUG_MINIMUM trues
//#define DEBUG_INITIAL_LEVEL DEBUG_LEVEL_VERBOSE

#define POT_THRESHOLD 5
#define PIN_POT 35

#define CAPACITIVE_TOUCH_THRESHOLD 11

#define PIN_RIGHT_SENSOR 12
#define PIN_LEFT_SENSOR 13 //12 is the correct one for the rest of the devices



#define FW_UPDATE_TIME 5000

float hue;
float potValue;
uint16_t lastPotValue;
uint32_t NumberOfLedsStrip = MAX_NUMBER_OF_LEDS;

void setup()
{
  ConfigManager_Initialize();
  
  NumberOfLedsStrip = ConfigManager_ReadParameter(PARAM_NUMBER_OF_LEDS);

  Serial.begin(115200);

  if (touchRead(PIN_LEFT_SENSOR) < CAPACITIVE_TOUCH_THRESHOLD)
  {
    while(touchRead(PIN_LEFT_SENSOR) < CAPACITIVE_TOUCH_THRESHOLD);
    MWST_InitializeStrip(STRIP_LEFT, NumberOfLedsStrip, 0, MAX_NUMBER_OF_LEDS);

    // If the pot is set to one side, we adjust the numbers of LEDs otherwise we go into upload mode
    if (analogRead(PIN_POT) > 2048)
    {
      uint8_t numberOfLEDs;
      
      while(1)
      {
        numberOfLEDs = map(analogRead(PIN_POT), 0, 4095, 1, MAX_NUMBER_OF_LEDS);
        MWST_SetLEDsColor(STRIP_LEFT, RgbwColor(0xFF, 0, 0xFF, 0), 0,  numberOfLEDs);
        if (touchRead(PIN_LEFT_SENSOR) < CAPACITIVE_TOUCH_THRESHOLD)
        {
          MWST_SetLEDsColor(STRIP_LEFT, RgbwColor(0, 0, 0, 0xFF), 0,  numberOfLEDs);
          //ConfigManager_Parameters().NumberOfLEDs = numberOfLEDs;
          ConfigManager_WriteParameter(PARAM_NUMBER_OF_LEDS,numberOfLEDs);
          ConfigManager_WriteConfigToEEPROM();
        }
      }
    }
    else
    {
      MWST_SetStripColor(STRIP_LEFT, RgbwColor(0xFF, 0, 0, 0));
      MWUP_EnterBootloaderMode();
    }
  }

  MWST_InitializeStrip(STRIP_LEFT, LEDS_PER_STRIP, 0, 51);

  MWIH_EnableInputSensor(MWIH_LEFT_SENSOR, PIN_LEFT_SENSOR);
  MWIH_EnableInputSensor(MWIH_RIGHT_SENSOR, PIN_RIGHT_SENSOR);
  MWSM_InitalizeStateMachine();
}

void loop()
{

  MWSM_RunStateMachine();
  // printlnD("Event: Left Sensor Released");
  // debugHandle();
  readPot();
}

void readPot()
{
  potValue = 0;
  for (int i = 0; i < 10; i++)
  {
    potValue += analogRead(PIN_POT);
  }
  potValue = potValue / 10;

  if (potValue < (lastPotValue - POT_THRESHOLD) || potValue > (lastPotValue + POT_THRESHOLD))
  {
    lastPotValue = potValue;

    hue = map(potValue, 0, 4095 - 2048, 0, 60000) / 60000.0;

    if (potValue < (4095 - 2048))
    {

      MWST_SetStripColor(STRIP_LEFT, RgbwColor(HsbColor(hue, 0.8f, 1.0f)));
    }
    else
    {

      MWST_SetStripColor(STRIP_LEFT, RgbwColor(0, 0, map(4095 - potValue, 0, 4095 - 2048, 0, 255), 255));
    }
  }
}
