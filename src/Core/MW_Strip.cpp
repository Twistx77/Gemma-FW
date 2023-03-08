#include "MW_Strip.h"
#include "../Configuration/ConfigManager.h"
#include "../DefaultConfig.h"

#define DELAY_EFFECT_PROGRESSIVE_MS 0
#define DELAY_EFFECT_RANDOM_MS 10

#define INCREASE_BRIGHTNESS true
#define DECREASE_BRIGHTNESS false

// bool increaseBrightness = true;

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

NeoPixelBrightnessBus<NeoRgbwFeature, Neo800KbpsMethod> *stripHW = NULL;

MWST_TypeStripConfig stripLeftCfg, stripRightCfg, stripCenterCfg;
MWST_TypeStripConfig strips[] = {stripCenterCfg, stripLeftCfg, stripRightCfg};

void effectFade(MWST_TypeStripConfig *strip, uint8_t firstLED, uint8_t lastLED)
{

  uint8_t step = 3;
  if (strip->currentState == MWST_ENABLED)
  {
    uint16_t i = 0;

    while (i < strip->setBrightness)
    {
      if (i <= (strip->setBrightness - step))
      {
        i += step;
      }
      else
      {
        i = strip->setBrightness;
        stripHW->SetBrightness(strip->setBrightness, firstLED, lastLED);
      }
      stripHW->ClearTo(strip->currentColor, firstLED, lastLED);
      stripHW->SetBrightness(i, firstLED, lastLED);
      stripHW->Show();
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
      stripHW->SetBrightness(i, firstLED, lastLED);
      stripHW->Show();
    }
  }
}

void effectProgressive(MWST_TypeStripConfig *strip, uint8_t firstLED, uint8_t lastLED, RgbwColor color)
{
  for (uint8_t i = firstLED; i <= lastLED; i++)
  {
    stripHW->SetPixelColor(i, color);
    stripHW->Show();
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
    stripHW->SetPixelColor(led, color);
    if (centerLED - led > 0)
    {
      stripHW->SetPixelColor(centerLED - led, color);
    }
    stripHW->Show();
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
    stripHW->SetPixelColor(leds_array[r], color);
    stripHW->Show();
    leds_array[r] = leds_array[max];
    max = -1;
    delay(DELAY_EFFECT_RANDOM_MS);
  }
}

void MWST_Initialize()
{
  ConfigManager configManager = ConfigManager::getInstance();

  uint16_t ledsInStrip = (uint16_t)configManager.getParameter(DefaultParametersConfig[ID_LEDS_STRIP]);
  uint8_t ledsNightLightLeft = (uint8_t)configManager.getParameter(DefaultParametersConfig[ID_LEDS_NL_LEFT]);
  uint8_t ledsNightLightRight = (uint8_t)configManager.getParameter(DefaultParametersConfig[ID_LEDS_NL_RIGHT]);

  strips[STRIP_CENTER].stripType = STRIP_CENTER;
  strips[STRIP_CENTER].currentState = MWST_DISABLED;
  strips[STRIP_CENTER].currentColor = RgbwColor(0, 0, 0, 255);
  strips[STRIP_CENTER].setBrightness = MAX_BRIGHTNESS;
  strips[STRIP_CENTER].currentBrightness = 0;
  strips[STRIP_CENTER].numberOfLEDs = ledsInStrip;
  strips[STRIP_CENTER].numLEDsStart = 0;
  strips[STRIP_CENTER].numLEDsStop = ledsInStrip - 1;
  strips[STRIP_CENTER].brightnessDir = INCREASE_BRIGHTNESS;

  strips[STRIP_LEFT].stripType = STRIP_LEFT;
  strips[STRIP_LEFT].currentState = MWST_DISABLED;
  strips[STRIP_LEFT].currentColor = RgbwColor(0, 0, 0, 255);
  strips[STRIP_LEFT].setBrightness = MAX_BRIGHTNESS;
  strips[STRIP_LEFT].currentBrightness = 0;
  strips[STRIP_LEFT].numberOfLEDs = ledsNightLightLeft;
  strips[STRIP_LEFT].numLEDsStart = 0;
  strips[STRIP_LEFT].numLEDsStop = ledsNightLightLeft - 1;
  strips[STRIP_LEFT].brightnessDir = INCREASE_BRIGHTNESS;

  strips[STRIP_RIGHT].stripType = STRIP_RIGHT;
  strips[STRIP_RIGHT].currentState = MWST_DISABLED;
  strips[STRIP_RIGHT].currentColor = RgbwColor(0, 0, 0, 255);
  strips[STRIP_RIGHT].setBrightness = MAX_BRIGHTNESS;
  strips[STRIP_RIGHT].currentBrightness = 0;
  strips[STRIP_RIGHT].numberOfLEDs = ledsNightLightRight;
  strips[STRIP_RIGHT].numLEDsStart = ledsInStrip - ledsNightLightRight;
  strips[STRIP_RIGHT].numLEDsStop = ledsInStrip - 1;
  strips[STRIP_RIGHT].brightnessDir = INCREASE_BRIGHTNESS;

  // Reasign pixelCount to the read number of pixels
  if (stripHW != NULL)
  {
    delete stripHW; // delete the previous dynamically created strip
  }
  stripHW = new NeoPixelBrightnessBus<NeoRgbwFeature, Neo800KbpsMethod>(ledsInStrip, PIN_STRIP_DEFAULT);

  if (stripHW == NULL)
  {
    return;
  }

  stripHW->Begin();
  stripHW->SetBrightness(255);
  stripHW->ClearTo(RgbwColor(0, 0, 0, 0));
  stripHW->Show();
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
    stripHW->ClearTo(color, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
    stripHW->Show();
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

void MWST_SetBrightness(uint8_t stripType, uint8_t new_brightness)
{
  strips[stripType].setBrightness = new_brightness;
  strips[stripType].currentBrightness = new_brightness;
  stripHW->ClearTo(strips[stripType].currentColor, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
  stripHW->SetBrightness(strips[stripType].currentBrightness, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);

  stripHW->Show();

  if (new_brightness > 0)
  {
    strips[stripType].currentState = MWST_ENABLED;
  }
  else
  {
    strips[stripType].currentState = MWST_DISABLED;
  }
}

void MWST_SetLEDsColor(uint8_t stripType, RgbwColor color, uint8_t firstLED, uint8_t lastLED)
{
  for (uint8_t i = firstLED; i <= lastLED; i++)
  {
    stripHW->SetPixelColor(i, color);
  }
  stripHW->Show();
}

void MWST_SetStripState(uint8_t stripType, bool state, uint8_t typeOfEffect)
{
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
      stripHW->ClearTo(RgbwColor(0, 0, 0, 0), strips[STRIP_LEFT].numLEDsStop + 1, strips[STRIP_CENTER].numLEDsStop);
      stripHW->Show();
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
      stripHW->ClearTo(RgbwColor(0, 0, 0, 0), 0, strips[STRIP_RIGHT].numLEDsStart - 1);
      stripHW->Show();
      strips[STRIP_RIGHT].setBrightness = strips[STRIP_CENTER].setBrightness;
      strips[STRIP_RIGHT].currentBrightness = strips[STRIP_CENTER].currentBrightness;
      strips[STRIP_CENTER].currentState = MWST_DISABLED;
      return;
    }

    break;

  default:
    return;
    break;
  }

  if (strips[stripType].currentState == MWST_ENABLED)
  {
    strips[stripType].currentBrightness = strips[stripType].setBrightness;
  }
  else
  {
    strips[stripType].currentBrightness = 0;
  }

  effectFade(&strips[stripType], strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
}

void MWST_ToggleStripState(uint8_t stripType)
{
  MWST_SetStripState(stripType, !strips[stripType].currentState, CURRENT_EFFECT);
}

/*
void MWST_IncreaseStripIlumination(uint8_t stripType, uint8_t steps)
{
  static uint32_t lastStepTime = millis();

  while (millis() < (lastStepTime + NL_BRIGHTNESS_CHANGE_DELAY_MS))
    ; // Wait until the step time delay has passed.
  lastStepTime = millis();

  // if ((strips[stripType].brightnessDir == INCREASE_BRIGHTNESS) && (strips[stripType].currentBrightness < (MAX_BRIGHTNESS - steps)))

  Serial.print("Set Brightness" + String(strips[stripType].setBrightness));
  if ((strips[stripType].setBrightness < (MAX_BRIGHTNESS / 2)) && (strips[stripType].currentBrightness < (MAX_BRIGHTNESS - steps)))
  {
    Serial.println(" Increasing Brightness");
    strips[stripType].currentState = MWST_ENABLED;
    strips[stripType].currentBrightness += steps;
    strips[stripType].setBrightness = strips[stripType].currentBrightness;
    stripHW->ClearTo(strips[stripType].currentColor, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
    stripHW->SetBrightness(strips[stripType].currentBrightness, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);

    stripHW->Show();
  }
  // else if ((strips[stripType].brightnessDir == DECREASE_BRIGHTNESS) && (strips[stripType].currentBrightness > 0))
  if ((strips[stripType].setBrightness > (MAX_BRIGHTNESS / 2)) && (strips[stripType].currentBrightness < (MAX_BRIGHTNESS - steps)))
  {
    Serial.println(" Decreasing Brightness");
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
    stripHW->ClearTo(strips[stripType].currentColor, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
    stripHW->SetBrightness(strips[stripType].setBrightness, strips[stripType].numLEDsStart, strips[stripType].numLEDsStop);
    stripHW->Show();
  }
}*/

uint8_t MWST_GetMaxBrightness()
{
  return MAX_BRIGHTNESS;
}