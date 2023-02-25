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
    ID_HW_VERSION,
    ID_LEDS_NL_LEFT,
    ID_LEDS_NL_RIGHT,
    ID_SATURATION_ENCODER,
    ID_LEDS_STRIP,
    ID_TOUCH_THRESHOLD,
    ID_ENCODER_RESOLUTION,    
    MAX_CONFIG_PARAMETERS
};

// Array to store the parameters key, default value, min value and max value
const ConfigParameter DefaultParametersConfig[] = {

    {"HW_VER", 0, 255, 3},
    {"NL_LEFT", 4, 1, 60},
    {"NL_RIGHT", 4, 1, 60},
    {"SAT_ENC", 60, 1, 100},
    {"LEDS_STRIP", 16, 0, 150},
    {"TOUCH_THD", 90, 20, 99},
    {"ENC_RES", 4, 1, 255}
    };

#endif // CONFIG_PARAMETERS_H_
