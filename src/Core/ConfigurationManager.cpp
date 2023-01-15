
// ConfigurationManager.cpp
#include "ConfigurationManager.h"

const char *const PARAMETERS_KEYS[]{
    "FW_MAJOR",
    "FW_MINOR",
    "FW_PATCH",
    "DEBUG_OUTPUT",
    "PIN_STRIP",
    "PIN_CENTER_TS",
    "PIN_LEFT_TS",
    "PIN_RIGHT_TS",
    "PIN_LED",
    "ROT_ENC_A_PIN",
    "ROT_ENC_B_PIN",
    "ROT_ENC_BTN_PIN",
    "ROT_ENC_STEPS",
    "ROT_ENC_ACCEL",
    "STRIP_LEDS",
    "NL_LEDS",
    "MAX_BRIGHTNESS",
    "TOUCH_THLD"};

const uint32_t DEFAULT_PARAMETERS_VALUES[]{
    0,   // FW_MAJOR
    5,   // FW_MINOR
    8,   // FW_PATCH
    1,   // DEBUG_OUTPUT
    16,  // PIN_STRIP
    27,  // PIN_CENTER_TS
    2,   // PIN_LEFT_TS
    4,   // PIN_RIGHT_TS
    19,  // PIN_LED
    25,  // ROTARY_ENCODER_A_PIN
    23,  // ROTARY_ENCODER_B_PIN
    26,  // ROTARY_ENCODER_BUTTON_PIN
    4,   // ROTARY_ENCODER_STEPS
    250, // ROTARY_ENCODER_ACCELERATION
    106, // NUMBER_OF_LEDS
    4,   // NUMBER_OF_NL_LEDS
    255, // MAX_BRIGHTNESS
    6,   // CAPTOUCH_THLD_BOOT

};

// Initialize the ConfigurationManager
void ConfigurationManager::initialize()
{
    preferences.begin("config", false);
    for (int i = 0; i < sizeof(PARAMETERS_KEYS) / sizeof(PARAMETERS_KEYS[0]); i++)
    {
        if (!preferences.isKey(PARAMETERS_KEYS[i]))
        {
            Serial.println("Key not found: " + String(PARAMETERS_KEYS[i]));
            preferences.putULong(PARAMETERS_KEYS[i], DEFAULT_PARAMETERS_VALUES[i]);
        }
    }
}

// Load default configuration parameters
void ConfigurationManager::loadDefaultParameters()
{
    for (int i = 0; i < sizeof(PARAMETERS_KEYS) / sizeof(PARAMETERS_KEYS[0]); i++)
    {
        preferences.putULong(PARAMETERS_KEYS[i], DEFAULT_PARAMETERS_VALUES[i]);
    }
}

// Read a configuration parameter
uint32_t ConfigurationManager::readParameter(ConfigParameter parameter)
{
    return preferences.getULong(PARAMETERS_KEYS[parameter], DEFAULT_PARAMETERS_VALUES[parameter]);
}

// Write a configuration parameter
void ConfigurationManager::writeParameter(ConfigParameter parameter, uint32_t value)
{
    String(preferences.putULong(PARAMETERS_KEYS[parameter], value));
}
