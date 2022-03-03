
#include "ConfigurationManager.h"
#include "EEPROM.h"


uint32_t ConfigParamaters[Number_Of_Parameters];

void ConfigManager_Initialize()
{
    EEPROM.begin(sizeof(ConfigParamaters));
    if(EEPROM.read(0) != FW_MAJOR || EEPROM.read(1) != FW_MINOR || EEPROM.read(0) != FW_PATCH )
    {
        ConfigManager_WriteConfigToEEPROM();
    }
    else
    {
        ConfigManager_ReadConfigFromEEPROM();
    }
}

void ConfigManager_WriteConfigToEEPROM()
{
    uint8_t *p;
    p = (uint8_t *)&ConfigParamaters;
    for (uint16_t address = 0; address < sizeof(ConfigParamaters); address++)
    {
        EEPROM.write(address, *p);
        p++;
    }
    EEPROM.commit();
}

void ConfigManager_ReadConfigFromEEPROM()
{
    uint8_t *p;
    p = (uint8_t *)&ConfigParamaters;
    for (uint16_t address = 0; address < sizeof(ConfigParamaters); address++)
    {
        *p = EEPROM.read(address);
        p++;
    }
}

void ConfigManager_WriteParameter(uint8_t paramId, uint32_t value)
{   

    ConfigParamaters[paramId]
}
