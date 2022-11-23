#include "HMIM_HMIManager.h"
#include "MW_Strip.h"
#include "AiEsp32RotaryEncoder.h"
#include "DefaultConfig.h"
#include "driver/touch_pad.h"
#include "soc/sens_periph.h"

// #define DEBUG_THIS_FILE
#include "PrettyDebug.h"

#define DEBOUNCE_TIME 1            // TODO: REPLACE CONFIG MANAGER
#define LONG_CLICK_TIME 1000       // TODO: REPLACE CONFIG MANAGER

#define LONG_TOUCH_DURATION_MS 1000 // TODO: REPLACE CONFIG MANAGER
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10) // TODO: REPLACE CONFIG MANAGER
#define TOUCH_THRESH_NO_USE   (0)
#define TOUCH_THRESHOLD_PRPORTION (0.8)

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

TouchEvent_T touchSensorsEvents[] = {NO_EVENT, NO_EVENT, NO_EVENT}; // TODO: Check if this is actually needed
TouchSensor_t touchSensorTypes[] = {TS_CENTER, TS_LEFT, TS_RIGHT};  // TODO: Remove

static const touch_pad_t sensorsInput[] = {TOUCH_PAD_NUM7, TOUCH_PAD_NUM2, TOUCH_PAD_NUM0}; // CENTER - LEFT - RIGHT

static uint16_t touchThresholds[MAX_TOUCH_SENSORS] = {0xFFFF, 0xFFFF, 0xFFFF}; // Started at Max to be adjusted after booting

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);

uint8_t rotaryMode = ROTARY_BRIGHTNESS_MODE;

const char *SENSOR_NAMES[] = {"Center", "Left", "Right"};

uint8_t lastSensorControlled = STRIP_CENTER;

// bool buttonLongPressed[] = {false, false, false};

// TODO: Check if these handlers are still needed. Implement one liner if possible
void clickHandler(TouchSensor_t &sensor)
{
    DEBUG_OK("Short Press %s Button", SENSOR_NAMES[sensor]);
    switch (sensor)
    {
    case TS_CENTER:
        MWST_ToggleStripState(STRIP_CENTER);
        lastSensorControlled = STRIP_CENTER;
        break;

    case TS_LEFT:
        MWST_ToggleStripState(STRIP_LEFT);
        lastSensorControlled = STRIP_LEFT;
        break;

    case TS_RIGHT:
        MWST_ToggleStripState(STRIP_RIGHT);
        lastSensorControlled = STRIP_RIGHT;
        break;

    default:
        DEBUG_ERROR("Unknown button ID %d", sensor);
        break;
    }
}
/*
void longClickDetectedHandler(TouchSensor_t &sensor)
{
    DEBUG_OK("Long Press %s Button Started", SENSOR_NAMES[sensor]);
    switch (sensor)
    {
    case TS_CENTER:
        // buttonLongPressed[CENTER_TS] = true;
        break;
    case TS_LEFT:
        break;
    case TS_RIGHT:
        break;
    default:
        DEBUG_ERROR("Unknown button ID %d", sensor);
        break;
    }
}

void longClickHandler(TouchSensor_t &sensor)
{
    DEBUG_OK("Long Press %s Button Ended", SENSOR_NAMES[sensor]);

    switch (sensor)
    {
    case TS_CENTER:
        // buttonLongPressed[CENTER_TS] = false;
        // WST_ToggleIncreaseBrightness(STRIP_CENTER);
        break;
    case TS_LEFT:
        buttonLongPressed[LEFT_TS] = false;
        MWST_ToggleIncreaseBrightness(STRIP_LEFT);
        break;
    case TS_RIGHT:
        buttonLongPressed[RIGHT_TS] = false;
        MWST_ToggleIncreaseBrightness(STRIP_RIGHT);
        break;
    default:
        DEBUG_ERROR("Unknown button ID %d", sensor);
        break;
    }
}*/

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}

TouchState_T readTouchSensorState(touch_pad_t sensorInput, uint16_t threshold)
{
    uint16_t touchValue;
    touch_pad_read_raw_data(sensorInput, &touchValue);
    Serial.println("Val: " + String(touchValue) + " trhld " + String(threshold));

    if (touchValue < threshold)
    {
        return (TOUCHED);
    }
    else
    {
        return (NOT_TOUCHED);
    }
}

void processTouchInputs()
{
    static TouchState_T lastState[MAX_TOUCH_SENSORS] = {NOT_TOUCHED, NOT_TOUCHED, NOT_TOUCHED};

    static uint32_t lastTimePressed[3] = {millis(), millis(), millis()};
    TouchState_T currentState;

    for (uint8_t sensor = 0; sensor < MAX_TOUCH_SENSORS; sensor++)
    {
        currentState = readTouchSensorState(sensorsInput[sensor], touchThresholds[sensor]);

        // Check if there is change
        if (currentState == TOUCHED)
        {
            if (lastState[sensor] == TOUCHED)
            {
                if (touchSensorsEvents[sensor] == NO_EVENT && (millis() - lastTimePressed[sensor]) > LONG_TOUCH_DURATION_MS)
                {
                    touchSensorsEvents[sensor] = LONG_TOUCH_EVENT;
                    // longClickDetectedHandler(touchSensorTypes[sensor]);
                }
            }
            else
            {
                lastTimePressed[sensor] = millis();
                // longClickHandler(stouchSensorTypes[sensor]);
            }

            lastState[sensor] = TOUCHED;
            clickHandler(touchSensorTypes[sensor]);
        }
        else
        {

            if (lastState[sensor] == TOUCHED)
            {
                lastState[sensor] = NOT_TOUCHED;
            }

            else
            {
                touchSensorsEvents[sensor] = NO_EVENT;
            }
        }
    }
}

void HMIM_Initialize()
{

    touch_pad_init();
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_0V);

    // Set measuring time for the Touch Sensor FSM to improve resolution for bigger capacitive surface
    touch_pad_set_meas_time(50, 20000);
    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
    uint16_t touchValue;
    for (uint8_t sensor = 0; sensor < MAX_TOUCH_SENSORS; sensor++)
    {
        touch_pad_config(sensorsInput[sensor], TOUCH_THRESH_NO_USE);

        delay(100); // Wait for configuration to settle

        touchThresholds[sensor] = touch_pad_read_filtered(sensorsInput[sensor], &touchValue) * TOUCH_THRESHOLD_PRPORTION;
    }

    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);

    bool circleValues = false;
    rotaryEncoder.setBoundaries(0, ROTARY_ENCODER_MAX_VALUE_BRIGHTNESS, circleValues);
    rotaryEncoder.setEncoderValue(MAX_BRIGHTNESS);
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
                rotaryEncoder.setBoundaries(0, ROTARY_ENCODER_MAX_VALUE_COLOR, false);
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
            MWST_SetBrightness(STRIP_CENTER, brightness);
            MWST_SetBrightness(STRIP_LEFT, brightness);
            MWST_SetBrightness(STRIP_RIGHT, brightness);
        }
        else
        {
            if (encoder_value < (ROTARY_ENCODER_MAX_VALUE_COLOR / 2))
            {
                float hue = encoder_value / (ROTARY_ENCODER_MAX_VALUE_COLOR / 2.0);
                MWST_SetStripColor(STRIP_CENTER, RgbwColor(HsbColor(hue, DEFAULT_SATURATION, 1.0f)));
                MWST_SetStripColor(STRIP_LEFT, RgbwColor(HsbColor(hue, DEFAULT_SATURATION, 1.0f)));
                MWST_SetStripColor(STRIP_RIGHT, RgbwColor(HsbColor(hue, DEFAULT_SATURATION, 1.0f)));
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

void HMIN_ProcessHMI()
{
    for (uint8_t sensorType = 0; sensorType < (sizeof(touchSensorTypes) / sizeof(touchSensorTypes[0])); sensorType++)
    {
        processTouchInputs();
        if (touchSensorsEvents[sensorType] == LONG_TOUCH_EVENT)
        {
            MWST_IncreaseStripIlumination(sensorType + 1, 1);
        }
    }
    MWIH_ReadRotaryEncoder();
}