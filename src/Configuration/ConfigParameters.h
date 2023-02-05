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
    ID_FW_MAJOR,
    ID_FW_MINOR,
    ID_FW_PATCH,
    ID_DEBUG_OUT,
    ID_PIN_STRIP,
    ID_PIN_CENTER_TS,
    ID_PIN_LEFT_TS,
    ID_PIN_RIGHT_TS,
    ID_PIN_LED,
    ID_ROT_A_PIN,
    ID_ROT_B_PIN,
    ID_ROT_BTN_PIN,
    ID_ROT_STEPS,
    ID_ROT_ACCEL,
    ID_STRIP_LEDS,
    ID_NL_LEDS,
    ID_MAX_BRIGHT,
    ID_TOUCH_THLD,
    MAX_CONFIG_PARAMETERS
};

// Array to store the parameters key, default value, min value and max value
const ConfigParameter DefaultConfigParameters[] = {
    {"FW_MAJ", 0, 0, 0xFF},
    {"FW_MIN", 5, 0, 0xFF},
    {"FW_PATCH", 9, 0, 0xFF},
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
