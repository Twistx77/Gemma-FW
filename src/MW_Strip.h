#include <stdint.h>
#include <NeoPixelBrightnessBus.h>

#ifndef _H_MW_STRIP
#define _H_MW_STRIP

#define MWST_DISABLED false
#define MWST_ENABLED true


#define STRIP_LEFT 0
#define STRIP_RIGHT 1
#define STRIP_BOTH 2

#define EFFECT_PROGRESIVE_ON 1




void MWST_InitializeStrip(uint8_t stripType, uint8_t numberOfLEDs, uint8_t numLEDsStart, uint8_t numLEDsStop);
void MWST_ToggleStripState(uint8_t stripType, uint8_t typeOfEffect);
void MWST_IncreaseStripIlumination(uint8_t stripType, uint8_t steps);
void MWST_SetStripColor(uint8_t stripType, RgbwColor color);
void MWST_SetStripState(uint8_t stripType, bool state, uint8_t typeOfEffect);

#endif
