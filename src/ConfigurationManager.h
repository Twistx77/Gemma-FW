#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "DefaultConfig.h"
#include "stdint.h"


typedef struct TypeConfigParameters
{
    uint8_t FWMajor = FW_MAJOR;
    uint8_t FWMinor = FW_MINOR;
    uint8_t FWPatch = FW_PATCH;
    uint8_t NumberOfLEDs = NUMBER_OF_LEDS;
};

void ConfigManager_Initialize();
TypeConfigParameters ConfigManager_Parameters();
void ConfigManager_ReadConfigFromEEPROM();
void ConfigManager_WriteConfigToEEPROM()


#endif
