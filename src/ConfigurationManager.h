#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "DefaultConfig.h"
#include "stdint.h"

enum Parameters
{
    FWMajor,
    FWMinor,
    FWPatch,
    NumberOfLeds,
    NumberOfParameters    
};

void ConfigManager_Initialize();
void ConfigManager_ReadConfigFromEEPROM();
void ConfigManager_WriteConfigToEEPROM();


#endif
