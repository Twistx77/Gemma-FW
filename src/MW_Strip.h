#include <stdint.h>
#include <NeoPixelBrightnessBus.h>

#ifndef _H_MW_STRIP
#define _H_MW_STRIP

#define MWST_DISABLED false
#define MWST_ENABLED true

#define STRIP_CENTER 0
#define STRIP_LEFT 1
#define STRIP_RIGHT 2

#define EFFECT_PROGRESIVE 1
#define EFFECT_PROGRESSIVE_FROM_CENTER 2
#define EFFECT_RANDOM_LED 3

void MWST_Initialize(void);
void MWST_ToggleStripState(uint8_t stripType);
void MWST_IncreaseStripIlumination(uint8_t stripType, uint8_t steps);
void MWST_SetStripColor(uint8_t stripType, RgbwColor color);
void MWST_SetLEDsColor(uint8_t stripType, RgbwColor color, uint8_t firstLED, uint8_t lastLED);
void MWST_SetStripState(uint8_t stripType, bool state, uint8_t typeOfEffect);
void MWST_SetBrightness(uint8_t stripType, uint8_t brightness);

#endif
