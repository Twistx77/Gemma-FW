#include "MW_Strip.h"
#include "DefaultConfig.h"
#include "ConfigurationManager.h"

#define DELAY_EFFECT_PROGRESSIVE_MS 0
#define DELAY_EFFECT_RANDOM_MS 10

#define INCREASE_BRIGHTNESS true
#define DECREASE_BRIGHTNESS false

// bool increaseBrightness = true;

uint8_t lastStripActive = STRIP_CENTER;

typedef struct
{
  uint8_t stripType;
  bool currentState;
  RgbwColor currentColor;
  uint32_t increaseColor;
  uint8_t setBrightness;
  uint8_t currentBrightness;
  uint8_t numberOfLEDs;
  uint8_t numLEDsStart;
  uint8_t numLEDsStop;
  uint8_t brightnessDir;
} MWST_TypeStripConfig;

NeoPixelBrightnessBus<NeoRgbwFeature, Neo800KbpsMethod> stripHW(LEDS_STRIP,PIN_STRIP);

MWST_TypeStripConfig stripLeftCfg, stripRightCfg, stripCenterCfg;
MWST_TypeStripConfig strips[] = {stripCenterCfg, stripLeftCfg, stripRightCfg};

uint8_t maxBrightness;

void effectFade(MWST_TypeStripConfig *strip, uint8_t firstLED, uint8_t lastLED)
{

  uint8_t step = 5;
  if (strip->currentState == MWST_ENABLED)
  {
    uint16_t i = 0;

    while (i < strip->setBrightness)
    {
      if (i <  step)
      {
        i += step;
      }
      else
      {
        Serial.println("Strip brightness: " + String(i) + " - " + String(strip->setBrightness));
        i = strip->setBrightness;
      }
      stripHW.ClearTo(strip->currentColor, firstLED, lastLED);
      stripHW.SetBrightness(i, firstLED, lastLED);
      stripHW.Show();
    }
  }
  else
  {
    uint16_t i = strip->setBrightness;


    while (i > 0)
    {
      if (i < step)
      {
        i = 0;
      }
      else
      {
        i -= step;
      }
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

  ConfigurationManager configManager = ConfigurationManager::getInstance();
  configManager.initialize();
  uint8_t ledsInStrip = (uint8_t)configManager.readParameter(PARAM_NUMBER_OF_LEDS);
  uint8_t ledsNightLight = (uint8_t)configManager.readParameter(PARAM_NUMBER_OF_NL_LEDS);
  uint8_t pinStrip = (uint8_t)configManager.readParameter(PARAM_PIN_STRIP);
  maxBrightness = (uint8_t)configManager.readParameter(PARAM_MAX_BRIGHTNESS);

  Serial.println("ledsInStrip: " + String(ledsInStrip));
  Serial.println("ledsNightLight: " + String(ledsNightLight));


  strips[STRIP_CENTER].stripType = STRIP_CENTER;
  strips[STRIP_CENTER].currentState = MWST_DISABLED;
  strips[STRIP_CENTER].currentColor = RgbwColor(0, 0, 0, 255);
  strips[STRIP_CENTER].setBrightness = maxBrightness;
  strips[STRIP_CENTER].currentBrightness = 0;
  strips[STRIP_CENTER].numberOfLEDs = ledsInStrip;
  strips[STRIP_CENTER].numLEDsStart = 0;
  strips[STRIP_CENTER].numLEDsStop = ledsInStrip - 1;
  strips[STRIP_CENTER].brightnessDir = INCREASE_BRIGHTNESS;

  strips[STRIP_LEFT].stripType = STRIP_LEFT;
  strips[STRIP_LEFT].currentState = MWST_DISABLED;
  strips[STRIP_LEFT].currentColor = RgbwColor(0, 0, 0, 255);
  strips[STRIP_LEFT].setBrightness = maxBrightness;
  strips[STRIP_LEFT].currentBrightness = 0;
  strips[STRIP_LEFT].numberOfLEDs = ledsNightLight;
  strips[STRIP_LEFT].numLEDsStart = 0;
  strips[STRIP_LEFT].numLEDsStop = ledsNightLight - 1;
  strips[STRIP_LEFT].brightnessDir = INCREASE_BRIGHTNESS;

  strips[STRIP_RIGHT].stripType = STRIP_RIGHT;
  strips[STRIP_RIGHT].currentState = MWST_DISABLED;
  strips[STRIP_RIGHT].currentColor = RgbwColor(0, 0, 0, 255);
  strips[STRIP_RIGHT].setBrightness = maxBrightness;
  strips[STRIP_RIGHT].currentBrightness = 0;
  strips[STRIP_RIGHT].numberOfLEDs = ledsNightLight;
  strips[STRIP_RIGHT].numLEDsStart = ledsInStrip - ledsNightLight;
  strips[STRIP_RIGHT].numLEDsStop = ledsInStrip - 1;
  strips[STRIP_RIGHT].brightnessDir = INCREASE_BRIGHTNESS;

  stripHW.Begin();
  stripHW.SetBrightness(255);
  stripHW.ClearTo(RgbwColor(0, 0, 0, 0));
  stripHW.Show();
}
void MWST_ToggleIncreaseBrightness(uint8_t stripType)
{
  strips[stripType].brightnessDir = !strips[stripType].brightnessDir;
  if (strips[stripType].setBrightness == 0)
  {
    strips[stripType].currentState = MWST_DISABLED;
  }
  else
  {
    strips[stripType].currentState = MWST_ENABLED;
  }
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

uint8_t MWST_GetCurrentBrightness(uint8_t stripType)
{
  return (strips[stripType].currentBrightness);
}

RgbwColor MWST_GetColor(uint8_t stripType)
{
  return (strips[stripType].currentColor);
}

uint32_t MWST_GetColorIndex(uint8_t stripType)
{
  if (strips[stripType].currentColor.W > 0)
  {
    return (ROTARY_ENCODER_MAX_VALUE_COLOR - strips[stripType].currentColor.B);
  }
  else
  {
    return (HsbColor(RgbColor(strips[stripType].currentColor.R, strips[stripType].currentColor.G, strips[stripType].currentColor.B)).H * 255);
  }
}

bool MWST_GetState(uint8_t stripType)
{
  return (strips[stripType].currentState);
}

void MWST_SetBrightness(uint8_t stripType, uint8_t brightness)
{
  if (strips[stripType].currentState == MWST_ENABLED)
  {
    strips[stripType].setBrightness = brightness;
    strips[stripType].currentBrightness = brightness;
    stripHW.SetBrightness(strips[stripType].setBrightness, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
    stripHW.ClearTo(strips[stripType].currentColor, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
    stripHW.Show();
  }
  else if (strips[STRIP_LEFT].currentState == MWST_DISABLED && strips[STRIP_CENTER].currentState == MWST_DISABLED && strips[STRIP_RIGHT].currentState == MWST_DISABLED)
  {
    MWST_ToggleStripState(STRIP_CENTER);
    strips[stripType].setBrightness = brightness;
    strips[stripType].currentBrightness = 0;
    stripHW.SetBrightness(strips[stripType].setBrightness, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
    stripHW.ClearTo(strips[stripType].currentColor, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
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
      stripHW.ClearTo(RgbwColor(0, 0, 0, 0), strips[STRIP_LEFT].numLEDsStop + 1, strips[STRIP_CENTER].numLEDsStop);
      stripHW.Show();
      strips[STRIP_LEFT].setBrightness = strips[STRIP_CENTER].setBrightness;
      strips[STRIP_LEFT].currentBrightness = strips[STRIP_CENTER].currentBrightness;
      strips[STRIP_CENTER].currentState = MWST_DISABLED;
      return;
    }

    break;

  case STRIP_RIGHT:
    strips[STRIP_RIGHT].currentState = state;
    if (strips[STRIP_CENTER].currentState == MWST_ENABLED)
    {
      stripHW.ClearTo(RgbwColor(0, 0, 0, 0), 0, strips[STRIP_RIGHT].numLEDsStart - 1);
      stripHW.Show();
      strips[STRIP_RIGHT].setBrightness = strips[STRIP_CENTER].setBrightness;
      strips[STRIP_RIGHT].currentBrightness = strips[STRIP_CENTER].currentBrightness;
      strips[STRIP_CENTER].currentState = MWST_DISABLED;
      return;
    }

    break;

  default:
    Serial.println("Unknown Strip Type " + String(stripType));
    return;
    break;
  }

  // Brightness should never be 0. It is either Off with a certain brightness set or ON
  if (strips[stripType].setBrightness == 0)
    strips[stripType].setBrightness = maxBrightness;

  if (state == MWST_ENABLED)
  {
    newColor = strips[stripType].currentColor;
    lastStripActive = stripType;
    strips[stripType].brightnessDir = DECREASE_BRIGHTNESS;
  }
  else
  {
    newColor = RgbwColor(0, 0, 0, 0);
    lastStripActive = STRIP_NONE;
    strips[stripType].brightnessDir = INCREASE_BRIGHTNESS;
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
  MWST_SetStripState(stripType, !strips[stripType].currentState, CURRENT_EFFECT);
}

void MWST_IncreaseStripIlumination(uint8_t stripType, uint8_t steps)
{
  static uint32_t lastStepTime = millis();

  while (millis() < (lastStepTime + NL_BRIGHTNESS_CHANGE_DELAY_MS))
    ; // Wait until the step time delay has passed.
  lastStepTime = millis();

  if ((strips[stripType].brightnessDir == INCREASE_BRIGHTNESS) && (strips[stripType].currentBrightness < (maxBrightness - steps)))
  {
    strips[stripType].currentState = MWST_ENABLED;
    strips[stripType].currentBrightness += steps;
    strips[stripType].setBrightness = strips[stripType].currentBrightness;
    stripHW.ClearTo(strips[stripType].currentColor, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
    stripHW.SetBrightness(strips[stripType].currentBrightness, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);

    stripHW.Show();
  }
  else if ((strips[stripType].brightnessDir == DECREASE_BRIGHTNESS) && (strips[stripType].currentBrightness > 0))
  {
    if (strips[stripType].currentBrightness <= steps)
    {
      strips[stripType].setBrightness = 0;
      strips[stripType].currentState = MWST_DISABLED;
    }
    else
    {
      strips[stripType].setBrightness -= steps;
    }
    strips[stripType].currentBrightness = strips[stripType].setBrightness;
    stripHW.ClearTo(strips[stripType].currentColor, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
    stripHW.SetBrightness(strips[stripType].setBrightness, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
    stripHW.Show();
  }
}

uint8_t MWST_GetMaxBrightness()
{
  return maxBrightness;
}