#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H


#include "stdint.h"

enum Parameters
{
    PARAM_FW_MAJOR,
    PARAM_FW_MINOR,
    PARAM_FW_PATCH,
    PARAM_NUMBER_OF_LEDS,
    NUMBER_OF_PARAMETERS    
};

void ConfigManager_Initialize();
void ConfigManager_ReadConfigFromEEPROM();
void ConfigManager_WriteConfigToEEPROM();
void ConfigManager_WriteParameter(uint8_t paramId, uint32_t value);
uint32_t ConfigManager_ReadParameter(uint8_t paramId);


#endif
