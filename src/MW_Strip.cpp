#include "MW_Strip.h"

#define PIN_STRIP 16

#define MAX_BRIGHTNESS 255

//#define TEST_BOARD

#if defined(TEST_BOARD)
#define LEDS_NL 3
#define LEDS_STRIP 20
#else
#define LEDS_NL 3
#define LEDS_STRIP 106
#endif

#define DELAY_EFFECT_PROGRESSIVE_MS 0
#define DELAY_EFFECT_RANDOM_MS 10

bool increaseBrightness = true;

uint8_t lastStripActive = STRIP_CENTER;

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

void effectFade(MWST_TypeStripConfig *strip, uint8_t firstLED, uint8_t lastLED)
{
  if (strip->currentState == MWST_ENABLED)
  {
    stripHW.SetBrightness(0);
    for (uint16_t i = 0; i <=strip->brightness; i++)
    {
      stripHW.ClearTo(strip->currentColor, firstLED, lastLED); 
      stripHW.SetBrightness(i, firstLED, lastLED);         
      stripHW.Show();
    }
  }
  else
  {
    for (int16_t i = strip->brightness; i >= 0 ; i--)
    {  
      stripHW.SetBrightness(i, firstLED, lastLED);
      stripHW.Show();       
    }    
  }
}

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
  Serial.println("Start LED: " + String(firstLED) + "Stop LED:" + String(lastLED));
  uint8_t centerLED = (lastLED - firstLED) / 2;
  if (centerLED % 2 != 0)
  {
    centerLED = +1;
  }

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
  strips[STRIP_CENTER].numLEDsStop = LEDS_STRIP-1;

  strips[STRIP_LEFT].stripType = STRIP_LEFT;
  strips[STRIP_LEFT].currentState = MWST_DISABLED;
  strips[STRIP_LEFT].currentColor = RgbwColor(0, 0, 0, 255);
  strips[STRIP_LEFT].brightness = 255;
  strips[STRIP_LEFT].numberOfLEDs = LEDS_NL;
  strips[STRIP_LEFT].numLEDsStart = 0;
  strips[STRIP_LEFT].numLEDsStop = LEDS_NL - 1;

  strips[STRIP_RIGHT].stripType = STRIP_RIGHT;
  strips[STRIP_RIGHT].currentState = MWST_DISABLED;
  strips[STRIP_RIGHT].currentColor = RgbwColor(0, 0, 0, 255);
  strips[STRIP_RIGHT].brightness = 255;
  strips[STRIP_RIGHT].numberOfLEDs = LEDS_NL;
  strips[STRIP_RIGHT].numLEDsStart = LEDS_STRIP - LEDS_NL;
  strips[STRIP_RIGHT].numLEDsStop = LEDS_STRIP-1;

  stripHW.Begin();
  stripHW.SetBrightness(255);
  stripHW.ClearTo(RgbwColor(0, 0, 0, 0));

  stripHW.Show();
}

void MWST_SetStripColor(uint8_t stripType, RgbwColor color)
{
  if (strips[stripType].currentState == MWST_DISABLED)
  {
    strips[stripType].currentColor = color;
    return;
  }
  else
  {
    strips[stripType].currentColor = color;
    stripHW.ClearTo(color, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
    stripHW.Show();
  }
}

uint8_t MWST_GetBrightness(uint8_t stripType)
{
  return (strips[stripType].brightness);
}

uint32_t MWST_GetColorIndex(uint8_t stripType)
{
  if (strips[stripType].currentColor.W > 0)
  {
    return (strips[stripType].currentColor.B);
  }
  else
  {
    return (HsbColor(RgbColor(strips[stripType].currentColor.R, strips[stripType].currentColor.G, strips[stripType].currentColor.B)).H * 255);
  }
}

uint8_t MWST_GetState(uint8_t stripType)
{
  return (strips[stripType].currentState);
}

void MWST_SetBrightness(uint8_t stripType, uint8_t brightness)
{
  if (strips[stripType].currentState == MWST_ENABLED)
  {
    strips[stripType].brightness = brightness;
    Serial.println("Brightness: " + String(brightness)+ " numLEDsStart: " + String(strips[stripType].numLEDsStart) + " numLEDsStop: " + String(strips[stripType].numLEDsStop));
    stripHW.SetBrightness(strips[stripType].brightness, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
    stripHW.ClearTo(strips[stripType].currentColor);
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

uint8_t MWST_GetLastStripActive()
{
  return lastStripActive;
}

void MWST_SetStripState(uint8_t stripType, bool state, uint8_t typeOfEffect)
{
  RgbwColor newColor;

  switch (stripType)
  {
  case STRIP_CENTER:
    strips[STRIP_CENTER].currentState = state;
    strips[STRIP_LEFT].currentState = MWST_DISABLED;
    strips[STRIP_RIGHT].currentState = MWST_DISABLED;
    break;

  case STRIP_LEFT:
    strips[STRIP_LEFT].currentState = state;
    if (strips[STRIP_CENTER].currentState == MWST_ENABLED)
    {
      if (strips[STRIP_RIGHT].currentState == MWST_DISABLED)
      {

        stripHW.ClearTo(RgbwColor(0, 0, 0, 0), 0, strips[STRIP_RIGHT].numLEDsStart);
        stripHW.Show();
      }
      else
      {
        stripHW.ClearTo(RgbwColor(0, 0, 0, 0), strips[STRIP_LEFT].numLEDsStop, LEDS_STRIP );
        stripHW.Show();
        
      }
      strips[STRIP_CENTER].currentState = MWST_DISABLED;
    }
    break;

  case STRIP_RIGHT:
    strips[STRIP_RIGHT].currentState = state;
    if (strips[STRIP_CENTER].currentState == MWST_ENABLED)
    {
      if (strips[STRIP_LEFT].currentState == MWST_DISABLED)
      {
        effectProgressive(&strips[STRIP_CENTER], 0, strips[STRIP_RIGHT].numLEDsStart, RgbwColor(0, 0, 0, 0));
      }
      else
      {
        effectProgressive(&strips[STRIP_CENTER], strips[STRIP_LEFT].numLEDsStop, strips[STRIP_RIGHT].numLEDsStart, RgbwColor(0, 0, 0, 0));
      }

      strips[STRIP_CENTER].currentState = MWST_DISABLED;
    }
    break;
  default:
    Serial.println("Unknown Strip");
    break;
  }

  if (state == MWST_ENABLED)
  {
    newColor = strips[stripType].currentColor;
    lastStripActive = stripType;
  }
  else
  {
    newColor = RgbwColor(0, 0, 0, 0);
    lastStripActive = STRIP_NONE;
  }

  switch (typeOfEffect)
  {
    
  case EFFECT_FADE:
    effectFade(&strips[stripType], strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
    break;
  case EFFECT_PROGRESSIVE:
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

void MWST_ToggleStripState(uint8_t stripType)
{
  MWST_SetStripState(stripType, !strips[stripType].currentState, EFFECT_FADE);
}

void MWST_IncreaseStripIlumination(uint8_t stripType, uint8_t steps)
{
  delay(20);

  Serial.print("Increase Strip Ilummination");

  if (increaseBrightness && (strips[stripType].brightness < (MAX_BRIGHTNESS - steps)))
  {
    Serial.println("IncreaseStripIlumination " + String(stripType) + " " + String(steps));
    strips[stripType].brightness += steps;
    Serial.println("stripType: " + String(stripType) + " brightness: " + String(strips[stripType].brightness)+ " strips[stripType].currentColor: " + String(strips[stripType].currentColor.R) + " " + String(strips[stripType].currentColor.G) + " " + String(strips[stripType].currentColor.B) + " " + String(strips[stripType].currentColor.W));
    stripHW.ClearTo(strips[stripType].currentColor);//, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop); 
    stripHW.SetBrightness(strips[stripType].brightness);// strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);         
    stripHW.Show();
    //MWST_SetBrightness(stripType, strips[stripType].brightness);

    if (strips[stripType].brightness >= MAX_BRIGHTNESS)
    {
      increaseBrightness = false;
    }
    strips[stripType].currentState = MWST_ENABLED;
  }
  else if ((increaseBrightness == false) && (strips[stripType].brightness > steps))
  {
    Serial.println("DecreaseStripIlumination " + String(stripType) + " " + String(steps));
    strips[stripType].brightness -= steps;
    //MWST_SetBrightness(stripType, strips[stripType].brightness);
    stripHW.ClearTo(strips[stripType].currentColor);//, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop); 
    stripHW.SetBrightness(strips[stripType].brightness);// strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);         
    stripHW.Show();

    if (strips[stripType].brightness == 0)
    {
      increaseBrightness = true;
      strips[stripType].currentState = MWST_DISABLED;
    }
    strips[stripType].currentState = MWST_ENABLED;
  }
}
