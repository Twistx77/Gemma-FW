
#ifndef CONFIGURATION_MANAGER_H
#define CONFIGURATION_MANAGER_H

#include <stdint.h>
#include <Preferences.h>

enum ConfigParameter
{
    PARAM_FW_MAJOR = 0,
    PARAM_FW_MINOR = 1,
    PARAM_FW_PATCH = 2,
    PARAM_DEBUG_OUTPUT = 3,
    PARAM_PIN_STRIP = 4,
    PARAM_PIN_CENTER_TS = 5,
    PARAM_PIN_LEFT_TS = 6,
    PARAM_PIN_RIGHT_TS = 7,
    PARAM_PIN_LED = 8,
    PARAM_ROTARY_ENCODER_A_PIN = 9,
    PARAM_ROTARY_ENCODER_B_PIN = 10,
    PARAM_ROTARY_ENCODER_BUTTON_PIN = 11,
    PARAM_ROTARY_ENCODER_STEPS = 12,
    PARAM_ROTARY_ENCODER_ACCELERATION = 13,
    PARAM_NUMBER_OF_LEDS = 14,
    PARAM_NUMBER_OF_NL_LEDS = 15,
    PARAM_MAX_BRIGHTNESS = 16,
    PARAM_CAPTOUCH_THLD_BOOT = 17,
    MAX_PARAMETERS = 18    
};

class ConfigurationManager
{
public:
    // Get the instance of the Configuration Manager
    static ConfigurationManager &getInstance()
    {
        //Constructed on first access
        static ConfigurationManager instance;
        return instance;
    }

    void initialize();
    void loadDefaultParameters();
    uint32_t readParameter(ConfigParameter parameter);
    void writeParameter(ConfigParameter parameter, uint32_t value);

private:

    // Private data members
    Preferences preferences;
};

#endif // CONFIGURATION_MANAGER_H