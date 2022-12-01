#include "HMIM_HMIManager.h"
#include "MW_Strip.h"
#include "AiEsp32RotaryEncoder.h"
#include "DefaultConfig.h"
#include "driver/touch_pad.h"
#include "soc/sens_periph.h"

// #define DEBUG_THIS_FILE
#include "PrettyDebug.h"

#define DEBOUNCE_TIME 50 // TODO: REPLACE CONFIG MANAGER

#define LONG_TOUCH_DURATION_MS 1000       // TODO: REPLACE CONFIG MANAGER
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10) // TODO: REPLACE CONFIG MANAGER
#define TOUCH_THRESH_NO_USE (0)
#define TOUCH_THRESHOLD_PROPORTION (0.7)
#define NEW_THRESHOLD_PROPORTION (0.05)
#define CURRENT_THRESHOLD_PROPORTION (1 - NEW_THRESHOLD_PROPORTION)

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

TouchState_T readTouchSensorState(uint8_t sensor)
{
    uint16_t sample;
    uint16_t touchValue = 0;

    // touch_pad_read_filtered(sensorInput, &touchValue);
    for (uint8_t i = 0; i < 5; i++)
    {
        // touch_pad_read_filtered(sensorsInput[sensor], &sample);
        touch_pad_read_raw_data(sensorsInput[sensor], &sample);
        touchValue += sample;
    }

     touchThresholds[sensor] = touchThresholds[sensor]* CURRENT_THRESHOLD_PROPORTION + touchValue * TOUCH_THRESHOLD_PROPORTION*NEW_THRESHOLD_PROPORTION;

    if (touchValue < touchThresholds[sensor])
    {

        //Serial.println("1," + String(sensorsInput[sensor]) + "," + String(touchValue) + "," + String(touchThresholds[sensor]));
        SerialBT.println("1," + String(sensorsInput[sensor]) + "," + String(touchValue) + "," + String(touchThresholds[sensor]));

        return (TOUCHED);
    }
    else
    {
       
        SerialBT.println ("0," + String(sensorsInput[sensor]) + "," + String(touchValue) + "," + String(touchThresholds[sensor]));
        //Serial.println("0," + String(sensorsInput[sensor]) + "," + String(touchValue) + "," + String(touchThresholds[sensor]));
        // touchThresholds[sensor] = touchThresholds[sensor] * CURRENT_THRESHOLD_PROPORTION +  touchValue * TOUCH_THRESHOLD_PROPORTION * NEW_THRESHOLD_PROPORTION;
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
                         MWST_ToggleIncreaseBrightness(sensor);
                         SerialBT.println("Long " + String(sensor));
                         // longClickDetectedHandler(touchSensorTypes[sensor]);
                     }
                     else if (touchSensorsEvents[sensor] == LONG_TOUCH_EVENT)
                     {
                         MWST_ToggleIncreaseBrightness(sensor);
                     }
                 }
            }
            else
            {

                clickHandler(touchSensorTypes[sensor]);
                lastState[sensor] = TOUCHED;
                lastTimePressed[sensor] = millis();
                // longClickHandler(stouchSensorTypes[sensor]);
            }
            /* if (touchSensorsEvents[sensor] == NOT_TOUCHED && (millis() - lastTimePressed[sensor]) > DEBOUNCE_TIME)
              {
                  lastState[sensor] = TOUCHED;
                  touchSensorsEvents[sensor] = TOUCHED;
                  Serial.println("Touched " + String(SENSOR_NAMES[sensor]));
                  clickHandler(touchSensorTypes[sensor]);
              }*/
        }
        else
        {
            if (lastState[sensor] == TOUCHED)
            {
                SerialBT.println("Touched " + String(sensor));
                lastState[sensor] = NOT_TOUCHED;
                touchSensorsEvents[sensor] == NO_EVENT;
                /*
            if (touchSensorsEvents[sensor] != LONG_TOUCH_EVENT)
            {
                Serial.println("Touched " + String(sensor));
                clickHandler(touchSensorTypes[sensor]);
                lastState[sensor] = NOT_TOUCHED;
                touchSensorsEvents[sensor] == NO_EVENT;
            } */
            }
            
            else
            {
                touchSensorsEvents[sensor] = NO_EVENT;
                lastState[sensor] == NOT_TOUCHED;
            }
            //Serial.println("Release " + String(sensor));
        }
    }
}

void HMIM_Initialize()
{
    touch_pad_init();
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW); // TOUCH_FSM_MODE_TIMER);
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_0V);

    // Set measuring time for the Touch Sensor FSM to improve resolution for bigger capacitive surface
    touch_pad_set_meas_time(50, 7000);
    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
    // touch_pad_filter_stop();
    // touch_pad_filter_delete();

    uint16_t touchValue = 0;
    uint16_t sample;
    for (uint8_t sensor = 0; sensor < MAX_TOUCH_SENSORS; sensor++)
    {
        touch_pad_config(sensorsInput[sensor], TOUCH_THRESH_NO_USE);

        delay(100); // Wait for configuration to settle

        for (uint8_t i = 0; i < 5; i++)
        {
            touch_pad_read_filtered(sensorsInput[sensor], &sample);
           // touch_pad_read_raw_data(sensorsInput[sensor], &sample);
            touchValue += sample;
        }

        // touch_pad_read_filtered(sensorsInput[sensor], &touchValue);

        touchThresholds[sensor] = touchValue * TOUCH_THRESHOLD_PROPORTION;
    }

    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);

    bool circleValues = false;
    rotaryEncoder.setBoundaries(0, ROTARY_ENCODER_MAX_VALUE_BRIGHTNESS, circleValues);
    rotaryEncoder.setEncoderValue(MAX_BRIGHTNESS);
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
            MWST_IncreaseStripIlumination(sensorType, 1);
        }
    }
    MWIH_ReadRotaryEncoder();
}