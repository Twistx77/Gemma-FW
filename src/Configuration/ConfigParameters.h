#ifndef CONFIG_PARAMETERS_H_
#define CONFIG_PARAMETERS_H_

#include <stdint.h>

// Structure to store the configuration parameter information
typedef struct ConfigParameter
{
    // Key name of the parameter
    const char *const key;
    // Default value of the parameter
    uint32_t defaultValue;
    // Minimum value of the parameter
    uint32_t minValue;
    // Maximum value of the parameter
    uint32_t maxValue;
} ConfigParameter;

// Enum to specify the parameter IDs
enum ParameterID
{
    ID_FW_VERSION,
    ID_FW_BUILD,Encoder Resolution
    ID_LEDS_NL_LEFT,
    ID_LEDS_NL_RIGHT,
    ID_HUE_ENCODER,
    ID_LEDS_STRIP,    
    MAX_CONFIG_PARAMETERS
};

// Array to store the parameters key, default value, min value and max value
const ConfigParameter DefaultConfigParameters[] = {
    {"FW_VER", 0, 0, 0x00000509},
    {"FW_BUILD", 0, 0, 0x00000000},
    {"DBG_OUT", 0, 0, 1},
    {"P_STRIP", 0, 0, 40},
    {"P_CENT_TS", 0, 0, 40},
    {"P_LEFT_TS", 0, 0, 40},
    {"P_RIGHT_TS", 0, 0, 40},
    {"P_LED", 0, 0, 40},
    {"ROT_A", 0, 0, 40},
    {"ROT_B", 0, 0, 40},
    {"ROT_BTN", 0, 0, 40},
    {"ROT_STEP", 4, 1, 0xFFFF},
    {"ROT_ACC", 250, 1, 0xFFFF},
    {"STRIP_LEDS", 120, 1, 300},
    {"NL_LED", 0, 1, 50},
    {"MAX_BRT", 0, 10, 255},
    {"TCH_THLD", 0, 1, 0xFFFF}};

#endif // CONFIG_PARAMETERS_H_
