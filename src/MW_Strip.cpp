#include "MW_Strip.h"

#define PIN_STRIP 17

#define LEDS_STRIP 51

#define MAX_BRIGHTNESS 255

#define LEDS_NL 5

#define DELAY_EFFECT_PROGRESSIVE_MS 3
#define DELAY_EFFECT_RANDOM_MS 10


extern bool increaseBrightness;

typedef struct
{
  uint8_t stripType;
  bool currentState;
  RgbwColor currentColor;
  uint32_t increaseColor;
  uint32_t brightness;
  uint8_t numberOfLEDs;
  uint8_t numLEDsStart;
  uint8_t numLEDsStop;
} MWST_TypeStripConfig;

NeoPixelBrightnessBus<NeoRgbwFeature, Neo800KbpsMethod> stripHW(LEDS_STRIP, PIN_STRIP);

MWST_TypeStripConfig stripLeftCfg, stripRightCfg, stripCenterCfg;
MWST_TypeStripConfig strips[] = {stripCenterCfg, stripLeftCfg, stripRightCfg};

void effectProgressive(MWST_TypeStripConfig *strip, uint8_t firstLED, uint8_t lastLED, RgbwColor color)
{
  for (uint8_t i = firstLED; i <= lastLED; i++)
  {
    stripHW.SetPixelColor(i, color);
    stripHW.Show();
    delay(DELAY_EFFECT_PROGRESSIVE_MS);
  }
}

void effectProgressiveFromCenter(MWST_TypeStripConfig *strip, uint8_t firstLED, uint8_t lastLED, RgbwColor color)
{
  uint8_t centerLED = (lastLED - firstLED) / 2;
  if (centerLED % 2 != 0)
  {
    centerLED = +1;
  }
  Serial.print("Center LED ");
  Serial.println(centerLED);
  for (uint8_t led = centerLED; led <= lastLED; led++)
  {
    stripHW.SetPixelColor(led, color);
    if (centerLED - led > 0)
    {
      stripHW.SetPixelColor(centerLED - led, color);
    }
    stripHW.Show();
    delay(DELAY_EFFECT_PROGRESSIVE_MS);
  }
}

void effectRandomLED(MWST_TypeStripConfig *strip, uint8_t firstLED, uint8_t lastLED, RgbwColor color)
{
  uint8_t leds_array[strip->numberOfLEDs];
  uint8_t max = strip->numberOfLEDs;
  uint8_t r = 0;

  randomSeed(millis());
  for (uint8_t i = 0; i <= strip->numberOfLEDs; i++)
  {
    leds_array[i] = i;
  }
  for (uint8_t i = 0; i <= strip->numberOfLEDs; i++)
  {
    r = random(max);
    stripHW.SetPixelColor(leds_array[r], color);
    stripHW.Show();
    leds_array[r] = leds_array[max];
    max = -1;
    delay(DELAY_EFFECT_RANDOM_MS);
  }
}

void MWST_Initialize()
{
  strips[STRIP_CENTER].stripType = STRIP_CENTER;
  strips[STRIP_CENTER].currentState = MWST_DISABLED;
  strips[STRIP_CENTER].currentColor = RgbwColor(0, 0, 0, 255);
  strips[STRIP_CENTER].brightness = 255;
  strips[STRIP_CENTER].numberOfLEDs = LEDS_STRIP;
  strips[STRIP_CENTER].numLEDsStart = 0;
  strips[STRIP_CENTER].numLEDsStop = LEDS_STRIP;

  strips[STRIP_LEFT].stripType = STRIP_LEFT;
  strips[STRIP_LEFT].currentState = MWST_DISABLED;
  strips[STRIP_LEFT].currentColor = RgbwColor(0, 0, 0, 255);
  strips[STRIP_LEFT].brightness = 255;
  strips[STRIP_LEFT].numberOfLEDs = LEDS_NL;
  strips[STRIP_LEFT].numLEDsStart = LEDS_STRIP - LEDS_NL;
  strips[STRIP_LEFT].numLEDsStop = LEDS_STRIP;

  strips[STRIP_RIGHT].stripType = STRIP_RIGHT;
  strips[STRIP_RIGHT].currentState = MWST_DISABLED;
  strips[STRIP_RIGHT].currentColor = RgbwColor(0, 0, 0, 255);
  strips[STRIP_RIGHT].brightness = 255;
  strips[STRIP_RIGHT].numberOfLEDs = LEDS_NL;
  strips[STRIP_RIGHT].numLEDsStart = 0;
  strips[STRIP_RIGHT].numLEDsStop = LEDS_NL;

  stripHW.Begin();
  stripHW.SetBrightness(255);
  stripHW.ClearTo(RgbwColor(0, 0, 0, 0));
  
  stripHW.Show();
}



void MWST_SetStripColor(uint8_t stripType, RgbwColor color)
{
  strips[stripType].currentColor = color;
  stripHW.ClearTo(color);
  stripHW.Show();
}

void MWST_SetBrightness(uint8_t stripType, uint8_t brightness)
{
  strips[STRIP_LEFT].brightness = brightness;

  stripHW.SetBrightness(strips[STRIP_LEFT].brightness);

  if (strips[stripType].currentState == MWST_ENABLED)
  {
    stripHW.ClearTo(strips[STRIP_LEFT].currentColor);

    stripHW.Show();
  }
}

void MWST_SetLEDsColor(uint8_t stripType, RgbwColor color, uint8_t firstLED, uint8_t lastLED)
{
  for (uint8_t i = firstLED; i <= lastLED; i++)
  {
    stripHW.SetPixelColor(i, color);
  }
  stripHW.Show();
}

void MWST_SetStripState(uint8_t stripType, bool state, uint8_t typeOfEffect)
{
  RgbwColor newColor;

  strips[stripType].currentState = state;

  if (state == MWST_ENABLED)
  {
    newColor = strips[stripType].currentColor;
    ;
  }
  else
  {
    newColor = RgbwColor(0, 0, 0, 0);
  }

  switch (typeOfEffect)
  {
  case EFFECT_PROGRESIVE:
    effectProgressive(&strips[stripType], strips[stripType].numLEDsStart, strips[stripType].numLEDsStop, newColor);
    break;

  case EFFECT_PROGRESSIVE_FROM_CENTER:
    effectProgressiveFromCenter(&strips[stripType], strips[stripType].numLEDsStart, strips[stripType].numLEDsStop, newColor);
    break;

  case EFFECT_RANDOM_LED:
    effectRandomLED(&strips[stripType], strips[stripType].numLEDsStart, strips[stripType].numLEDsStop, newColor);
    break;

  default:

    break;
  }
}

/*
void MWST_ToggleStripState(uint8_t stripType, uint8_t typeOfEffect)
{
  MWST_SetStripState(stripType, !strips[stripType].currentState, typeOfEffect);
}*/

void MWST_ToggleStripState(uint8_t stripType)
{
  MWST_SetStripState(stripType, !strips[stripType].currentState, EFFECT_PROGRESSIVE_FROM_CENTER);
}

void MWST_IncreaseStripIlumination(uint8_t stripType, uint8_t steps)
{
  delay(5);

  if (increaseBrightness && (strips[STRIP_LEFT].brightness < (MAX_BRIGHTNESS - steps)))
  {
    strips[STRIP_LEFT].brightness += steps;
    stripHW.ClearTo(strips[STRIP_LEFT].currentColor);

    stripHW.SetBrightness(strips[STRIP_LEFT].brightness);

    stripHW.Show();

    if (strips[STRIP_LEFT].brightness >= MAX_BRIGHTNESS)
    {
      increaseBrightness = false;
    }
    strips[stripType].currentState = MWST_ENABLED;
  }
  else if ((increaseBrightness == false) && (strips[STRIP_LEFT].brightness > (0 + steps)))
  {
    strips[STRIP_LEFT].brightness -= steps;
    stripHW.ClearTo(strips[STRIP_LEFT].currentColor);

    stripHW.SetBrightness(strips[STRIP_LEFT].brightness);

    stripHW.Show();

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