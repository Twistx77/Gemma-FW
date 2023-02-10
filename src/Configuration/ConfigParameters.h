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

// Enum to specify the device info parameter IDs
enum ParameterID
{
    ID_FW_VERSION,
    ID_FW_BUILD,
    ID_LEDS_NL_LEFT,
    ID_LEDS_NL_RIGHT,
    ID_HUE_ENCODER,
    ID_LEDS_STRIP,
    ID_TOUCH_THRESHOLD,
    ID_ENCODER_RESOLUTION,    
    MAX_CONFIG_PARAMETERS
};

// Array to store the parameters key, default value, min value and max value
const ConfigParameter DefaultParametersConfig[] = {
    {"FW_VER", 0, 0, 0xDEADBEEF},
    {"FW_BUILD", 0, 0, 0x000000000},

    {"NL_LEFT", 0, 0, 0xFFFFFFFF},
    {"NL_RIGHT", 0, 0, 0xFFFFFFFF},
    {"HUE_ENC", 0, 0, 0xFFFFFFFF},
    {"LEDS_STRIP", 0, 0, 0xFFFFFFFF},
    {"TOUCH_THD", 0, 0, 0xFFFFFFFF},
    {"ENC_RES", 0, 0, 0xFFFFFFFF}
    };

#endif // CONFIG_PARAMETERS_H_
