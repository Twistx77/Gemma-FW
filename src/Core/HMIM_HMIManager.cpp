#include "MW_Strip.h"
#include "HMIM_HMIManager.h"

#include "driver/touch_pad.h"
#include "soc/sens_periph.h"

#include "../DefaultConfig.h"
#include "../Drivers/AiEsp32RotaryEncoder.h"
#include "../Configuration/ConfigManager.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DEBOUNCE_TIME 50 // TODO: REPLACE CONFIG MANAGER

#define LONG_TOUCH_DURATION_MS 1200       // TODO: REPLACE CONFIG MANAGER
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10) // TODO: REPLACE CONFIG MANAGER
#define TOUCH_THRESH_NO_USE (0)
#define NEW_THRESHOLD_PROPORTION (0.001)

// TODO: Check if this is actually needed
typedef enum
{
    NO_EVENT,
    TOUCH_EVENT,
    LONG_TOUCH_EVENT
} TouchEvent_T;

typedef enum
{
    NOT_TOUCHED,
    TOUCHED
} TouchState_T;

typedef enum
{
    NO_CHANGE,
    INCREASE,
    DECREASE
} BrightnessDirectionChange_T;

TouchEvent_T touchSensorsEvents[] = {NO_EVENT, NO_EVENT, NO_EVENT}; // TODO: Check if this is actually needed
TouchSensor_t touchSensorTypes[] = {TS_CENTER, TS_LEFT, TS_RIGHT};  // TODO: Remove
BrightnessDirectionChange_T changeStripBrightnessDirection[] = {NO_CHANGE, NO_CHANGE, NO_CHANGE};

touch_pad_t sensorsInput[] = {TOUCH_PAD_NUM7, TOUCH_PAD_NUM2, TOUCH_PAD_NUM0}; // CENTER - LEFT - RIGHT

static float touchThresholds[MAX_TOUCH_SENSORS] = {0xFFFF, 0xFFFF, 0xFFFF}; // Started at Max to be adjusted after booting

AiEsp32RotaryEncoder rotaryEncoder;

uint8_t rotaryMode = ROTARY_BRIGHTNESS_MODE;

const char *SENSOR_NAMES[] = {"Center", "Left", "Right"};

uint8_t lastSensorControlled = STRIP_CENTER;

float colorSaturationEncoder;

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}

TouchState_T readTouchSensorState(uint8_t sensor)
{
    uint16_t touchValue;
    touch_pad_read_raw_data(sensorsInput[sensor], &touchValue);

    if (touchValue < touchThresholds[sensor])
    {
        return (TOUCHED);
    }
    else
    {
        return (NOT_TOUCHED);
    }
}

void updateStripBrightness(uint8_t strip)
{
    if (changeStripBrightnessDirection[strip] == NO_CHANGE)
    {
        return;
    }
    int16_t newBrightness = 0;
    uint8_t currentBrightness = MWST_GetCurrentBrightness(strip);

    if (changeStripBrightnessDirection[strip] == INCREASE) // Increse brightness
    {
        newBrightness = currentBrightness + TOUCH_BRIGHTNESS_STEP;
        if (newBrightness > MAX_BRIGHTNESS)
            newBrightness = MAX_BRIGHTNESS;
    }
    else // Decrease brightness
    {
        newBrightness = currentBrightness - TOUCH_BRIGHTNESS_STEP;
        if (newBrightness < 1)
            newBrightness = 1;
    }
    // Update the new brightness
    MWST_SetBrightness(strip, newBrightness);

    // Update encoder with new brightness so there is no "jump" in brightness
    rotaryEncoder.setEncoderValue(MWST_GetCurrentBrightness(STRIP_CENTER));
    // Read the encoder to ignore the change since it is not an update from the user.
    // TODO: Find a better way 
    rotaryEncoder.encoderChanged();
}

void processTouchInputs()
{
    static TouchState_T lastState[MAX_TOUCH_SENSORS] = {NOT_TOUCHED, NOT_TOUCHED, NOT_TOUCHED};

    static uint32_t lastTimePressed[3] = {millis(), millis(), millis()};
    TouchState_T currentState;

    for (uint8_t sensor = 0; sensor < MAX_TOUCH_SENSORS; sensor++)
    {
        currentState = readTouchSensorState(sensor);

        // Check if there is change
        if (currentState == TOUCHED)
        {
            if (lastState[sensor] == TOUCHED)
            {
                if ((millis() - lastTimePressed[sensor]) > LONG_TOUCH_DURATION_MS)
                {
                    if (touchSensorsEvents[sensor] == NO_EVENT)
                    {
                        touchSensorsEvents[sensor] = LONG_TOUCH_EVENT;
                        // If current brightness is lower than 25% the brightness, increase the brightness otherwise, decrease it
                        if (MWST_GetCurrentBrightness(sensor) <= (MAX_BRIGHTNESS / 4))
                            changeStripBrightnessDirection[sensor] = INCREASE;
                        else
                            changeStripBrightnessDirection[sensor] = DECREASE;
                    }
                }
            }
            else
            {
                lastState[sensor] = TOUCHED;
                lastTimePressed[sensor] = millis();
            }
        }
        else
        {
            if (lastState[sensor] == TOUCHED)
            {
                if (touchSensorsEvents[sensor] != LONG_TOUCH_EVENT)
                {
                    MWST_ToggleStripState(sensor);
                }
                else
                {
                    changeStripBrightnessDirection[sensor] = NO_CHANGE;
                }
                lastState[sensor] = NOT_TOUCHED;
                touchSensorsEvents[sensor] = NO_EVENT;
            }
            else
            {
                touchSensorsEvents[sensor] = NO_EVENT;
                lastState[sensor] = NOT_TOUCHED;
            }
        }
    }
}

void HMIM_Initialize()
{
    // Read configuration
    ConfigManager configManager = ConfigManager::getInstance();

    colorSaturationEncoder = (uint8_t)configManager.getParameter(DefaultParametersConfig[ID_SATURATION_ENCODER]) / 100.0;
    float capTouchThreshold = (uint8_t)configManager.getParameter(DefaultParametersConfig[ID_TOUCH_THRESHOLD]) / 100.0;
    uint8_t encoderSteps = (uint8_t)configManager.getParameter(DefaultParametersConfig[ID_ENCODER_RESOLUTION]);

    // Initialize Touch Sensors
    touch_pad_init();
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW);
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_0V);

    // Set measuring time for the Touch Sensor FSM to improve resolution for bigger capacitive surface
    // touch_pad_set_meas_time(50, 7000);
    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);

    uint16_t touchValue = 0;
    for (uint8_t sensor = 0; sensor < MAX_TOUCH_SENSORS; sensor++)
    {
        touch_pad_config(sensorsInput[sensor], TOUCH_THRESH_NO_USE);

        delay(100); // Wait for configuration to settle

        touch_pad_read_filtered(sensorsInput[sensor], &touchValue);

        touchThresholds[sensor] = touchValue * capTouchThreshold;
    }

    rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, encoderSteps);

    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);

    bool circleValues = false;
    rotaryEncoder.setBoundaries(1, ROTARY_ENCODER_MAX_VALUE_BRIGHTNESS, circleValues);
    rotaryEncoder.setEncoderValue(MWST_GetMaxBrightness());
    rotaryEncoder.encoderChanged();
    rotaryMode = ROTARY_BRIGHTNESS_MODE;
}

void MWIH_ReadRotaryEncoder()
{
    if (rotaryEncoder.isEncoderButtonClicked())
    {
        static unsigned long lastTimePressed = 0;
        // ignore multiple press in that time milliseconds
        if (millis() - lastTimePressed > 500)
        {
            lastTimePressed = millis();

            if (rotaryMode == ROTARY_BRIGHTNESS_MODE)
            {
                rotaryMode = ROTARY_COLOR_MODE;
                rotaryEncoder.setBoundaries(1, ROTARY_ENCODER_MAX_VALUE_COLOR, false);
                rotaryEncoder.setEncoderValue(MWST_GetColorIndex(STRIP_CENTER));
            }
            else
            {
                rotaryMode = ROTARY_BRIGHTNESS_MODE;
                rotaryEncoder.setBoundaries(0, ROTARY_ENCODER_MAX_VALUE_BRIGHTNESS, false);
                rotaryEncoder.setEncoderValue(MWST_GetCurrentBrightness(STRIP_CENTER));
            }
        }
    }
    if (rotaryEncoder.encoderChanged())
    {
        uint16_t encoder_value = rotaryEncoder.readEncoder();
        if (rotaryMode == ROTARY_BRIGHTNESS_MODE)
        {
            uint8_t brightness = encoder_value;
            
            MWST_SetBrightness(STRIP_LEFT, brightness);
            MWST_SetBrightness(STRIP_RIGHT, brightness);
            MWST_SetBrightness(STRIP_CENTER, brightness);
        }
        else
        {
            if (encoder_value < (ROTARY_ENCODER_MAX_VALUE_COLOR / 2))
            {
                float hue = encoder_value / (ROTARY_ENCODER_MAX_VALUE_COLOR / 2.0);
                
                MWST_SetStripColor(STRIP_LEFT, RgbwColor(HsbColor(hue, colorSaturationEncoder, 1.0f)));
                MWST_SetStripColor(STRIP_RIGHT, RgbwColor(HsbColor(hue, colorSaturationEncoder, 1.0f)));
                MWST_SetStripColor(STRIP_CENTER, RgbwColor(HsbColor(hue, colorSaturationEncoder, 1.0f)));
            }
            else
            {
                RgbwColor Color = RgbwColor(0, 0, map(encoder_value - (ROTARY_ENCODER_MAX_VALUE_COLOR / 2), (ROTARY_ENCODER_MAX_VALUE_COLOR / 2), 0, 0, 255), 255);
                MWST_SetStripColor(STRIP_CENTER, Color);
                MWST_SetStripColor(STRIP_LEFT, Color);
                MWST_SetStripColor(STRIP_RIGHT, Color);
            }
        }
    }
}

void HMIM_ProcessHMI()
{
    for (uint8_t sensorType = 0; sensorType < (sizeof(touchSensorTypes) / sizeof(touchSensorTypes[0])); sensorType++)
    {
        processTouchInputs();
        // Check if brightness has to be updated due to a long touch
        updateStripBrightness(sensorType);
    }

    MWIH_ReadRotaryEncoder();
}