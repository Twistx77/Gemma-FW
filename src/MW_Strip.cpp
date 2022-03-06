
#include "MW_Strip.h"

#define PIN_STRIP_LEFT 17
#define PIN_STRIP_RIGHT 17

#define LEDS_PER_STRIP 51

#define MAX_BRIGHTNESS 255

bool bothStripsEnabled = MWST_DISABLED;

extern bool increaseBrightness;

typedef struct
{
  uint8_t stripType;
  bool doubleStrip;
  bool currentState;
  RgbwColor currentColor;
  uint32_t increaseColor;
  uint32_t brightness;
  uint8_t numberOfLEDs;
  uint8_t numLEDsStart;
  uint8_t numLEDsStop;
  uint16_t delayAnimation;
} MWST_TypeStripConfig;

NeoPixelBrightnessBus<NeoRgbwFeature, Neo800KbpsMethod> stripLeft(LEDS_PER_STRIP, PIN_STRIP_LEFT);
NeoPixelBrightnessBus<NeoRgbwFeature, Neo800KbpsMethod> stripRight(LEDS_PER_STRIP, PIN_STRIP_RIGHT);

NeoPixelBrightnessBus<NeoRgbwFeature, Neo800KbpsMethod> stripsHW[] = {stripLeft, stripRight};

MWST_TypeStripConfig stripLeftCfg, stripRightCfg, stripBothCfg;
MWST_TypeStripConfig strips[] = {stripLeftCfg, stripRightCfg, stripBothCfg};

void effectProgressive(MWST_TypeStripConfig *strip, uint8_t firstLED, uint8_t lastLED, RgbwColor color, uint16_t waitBetweenLeds)
{

  if (strip->doubleStrip)
  {
    for (uint8_t i = firstLED; i <= lastLED; i++)
    {
      stripLeft.SetPixelColor(i, color);
      stripRight.SetPixelColor(i, color);
      stripLeft.Show();
      stripRight.Show();
      delay(waitBetweenLeds);
    }
  }
  else
  {
    for (uint8_t i = firstLED; i <= lastLED; i++)
    {
      stripLeft.SetPixelColor(i, color);
      stripLeft.Show();
      delay(waitBetweenLeds);
    }
  }
}

void effectProgressiveFromCenter(MWST_TypeStripConfig *strip, uint8_t firstLED, uint8_t lastLED, RgbwColor color, uint16_t waitBetweenLeds)
{
  uint8_t centerLED = (lastLED - firstLED)/2;
  if (centerLED % 2 != 0)
  {
    centerLED=+1;
  }
  Serial.print("Center LED ");
  Serial.println(centerLED);
  for (uint8_t led = centerLED; led <= lastLED; led++)
  {
    stripLeft.SetPixelColor(led, color);
    if (centerLED-led>0)
    {
      stripLeft.SetPixelColor(centerLED-led, color);
    }
    stripLeft.Show();
    delay(waitBetweenLeds);
  }
}



void effectRandomLED(MWST_TypeStripConfig *strip, uint8_t firstLED, uint8_t lastLED, RgbwColor color, uint16_t waitBetweenLeds)
{
  uint8_t leds_array[strip->numberOfLEDs];
  uint8_t max = strip->numberOfLEDs;
  uint8_t r = 0;

  randomSeed(millis());
  for (uint8_t i=0;  i<= strip->numberOfLEDs; i++ )
  {
    leds_array[i] = i;
  }
  for (uint8_t i=0;  i<= strip->numberOfLEDs; i++ )
  {
    r = random(max);  
    stripLeft.SetPixelColor(leds_array[r], color);
    stripLeft.Show();  
    leds_array[r] = leds_array [max];
    max =-1;
    delay(waitBetweenLeds);
  }
  
}

void MWST_InitializeStrip(uint8_t stripType, uint8_t numberOfLEDs, uint8_t numLEDsStart, uint8_t numLEDsStop)
{
  strips[stripType].stripType = stripType;
  strips[stripType].currentState = MWST_DISABLED;
  strips[stripType].currentColor = RgbwColor(0, 0, 0, 255);
  strips[stripType].brightness = 255;
  strips[stripType].numberOfLEDs = numberOfLEDs;
  strips[stripType].numLEDsStart = numLEDsStart;
  strips[stripType].numLEDsStop = numLEDsStop;
  strips[stripType].delayAnimation = 0;

  if (stripType == STRIP_BOTH)
  {
    strips[stripType].doubleStrip = true;
    stripRight.Begin();
    stripRight.SetBrightness(strips[stripType].brightness);
    stripRight.ClearTo(RgbwColor(0, 0, 0, 0));
    stripRight.Show();
  }

  stripLeft.Begin();
  stripLeft.SetBrightness(255);
  stripLeft.ClearTo(RgbwColor(0, 0, 0, 0));
  // stripLeft.fill(strips[stripType].currentColor, 0, numberOfLEDs);
  stripLeft.Show();
}

/*void MWST_SetStripColor(uint8_t stripType, uint32_t color)
  {
  strips[stripType].currentColor = color;

  stripLeft.ClearTo(color);
  stripLeft.Show();
  }*/

void MWST_SetStripColor(uint8_t stripType, RgbwColor color)
{
  strips[stripType].currentColor = color;
  stripLeft.ClearTo(color);
  stripLeft.Show();

}

void MWST_SetLEDsColor(uint8_t stripType, RgbwColor color, uint8_t firstLED, uint8_t lastLED)
{
    for (uint8_t i = firstLED; i <= lastLED; i++)
    {
      stripLeft.SetPixelColor(i, color);     
    }
    stripLeft.Show();
}

void MWST_SetStripState(uint8_t stripType, bool state, uint8_t typeOfEffect)
{
  RgbwColor newColor;

  strips[stripType].currentState = state;

  if (state == MWST_ENABLED)
  {
    newColor = strips[stripType].currentColor;
    //increaseBrightness = false;
  }
  else
  {
    newColor = RgbwColor(0, 0, 0, 0);
    // strips[stripType].brightness = 0;
    //increaseBrightness = true;
  }

  switch (typeOfEffect)
  {
  case EFFECT_PROGRESIVE:
    effectProgressive(&strips[stripType], strips[stripType].numLEDsStart, strips[stripType].numLEDsStop, newColor, strips[stripType].delayAnimation);
    break;

  case EFFECT_PROGRESSIVE_FROM_CENTER:
    effectProgressiveFromCenter(&strips[stripType], strips[stripType].numLEDsStart, strips[stripType].numLEDsStop, newColor, strips[stripType].delayAnimation);
  break;

  case EFFECT_RANDOM_LED:
    effectRandomLED(&strips[stripType], strips[stripType].numLEDsStart, strips[stripType].numLEDsStop, newColor, strips[stripType].delayAnimation);
  break;

  default:
  
    break;
  }
}

void MWST_ToggleStripState(uint8_t stripType, uint8_t typeOfEffect)
{
  MWST_SetStripState(stripType, !strips[stripType].currentState, typeOfEffect);
}

void MWST_IncreaseStripIlumination(uint8_t stripType, uint8_t steps)
{
  delay(5);

  if (increaseBrightness && (strips[STRIP_LEFT].brightness < (MAX_BRIGHTNESS - steps)))
  {
    strips[STRIP_LEFT].brightness += steps;
    stripLeft.ClearTo(strips[STRIP_LEFT].currentColor);

    stripLeft.SetBrightness(strips[STRIP_LEFT].brightness);

    stripLeft.Show();

    if (strips[STRIP_LEFT].brightness >= MAX_BRIGHTNESS)
    {
      increaseBrightness = false;
    }
    strips[stripType].currentState = MWST_ENABLED;
  }
  else if ((increaseBrightness == false) && (strips[STRIP_LEFT].brightness > (0 + steps)))
  {
    strips[STRIP_LEFT].brightness -= steps;
    stripLeft.ClearTo(strips[STRIP_LEFT].currentColor);

    stripLeft.SetBrightness(strips[STRIP_LEFT].brightness);

    stripLeft.Show();

    if (strips[STRIP_LEFT].brightness == 0)
    {
      increaseBrightness = true;
      strips[stripType].currentState = MWST_DISABLED;
    }
    strips[stripType].currentState = MWST_ENABLED;
  }
}
/*
void MWST_IncreaseStripIlumination(uint8_t stripType, uint8_t steps)
{
  delay(5);

  if ((strips[STRIP_LEFT].brightness < (MAX_BRIGHTNESS - steps)))
  {
    strips[STRIP_LEFT].brightness += steps;
    stripLeft.ClearTo(strips[STRIP_LEFT].currentColor);

    stripLeft.SetBrightness(strips[STRIP_LEFT].brightness);

    stripLeft.Show();

    strips[stripType].currentState = MWST_ENABLED;
  }
  else if (strips[STRIP_LEFT].brightness > (0 + steps))
  {
    strips[STRIP_LEFT].brightness -= steps;
    stripLeft.ClearTo(strips[STRIP_LEFT].currentColor);

    stripLeft.SetBrightness(strips[STRIP_LEFT].brightness);

    stripLeft.Show();

    if (strips[STRIP_LEFT].brightness == 0)
    {
      strips[stripType].currentState = MWST_DISABLED;
    }

    strips[stripType].currentState = MWST_ENABLED;
  }
}

*/