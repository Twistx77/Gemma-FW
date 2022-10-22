#include "HMIM_HMIManager.h"
#include "MW_Strip.h"
#include "Button2.h"
#include "AiEsp32RotaryEncoder.h"
#include "DefaultConfig.h"

//#define DEBUG_THIS_FILE 
#include "PrettyDebug.h"

#define CENTER_TS 0
#define LEFT_TS 1
#define RIGHT_TS 2



#define DEBOUNCE_TIME 1     // TODO: REPLACE CONFIG MANAGER
#define LONG_CLICK_TIME 1000 // TODO: REPLACE CONFIG MANAGER

#define ROTARY_ENCODER_A_PIN 25      // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_B_PIN 23      // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_BUTTON_PIN 26 // TODO: REPLACE CONFIG MANAGER

#define ROTARY_ENCODER_STEPS 4 // TODO: REPLACE CONFIG MANAGER

#define ROTARY_BRIGHTNESS_MODE 0                  // TODO: REPLACE CONFIG MANAGER
#define ROTARY_COLOR_MODE 1                       // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_MAX_VALUE_BRIGHTNESS 255.0 // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_MAX_VALUE_COLOR 512.0      // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_ACCELERATION 250           // TODO: REPLACE CONFIG MANAGER

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);

uint8_t rotaryMode = ROTARY_BRIGHTNESS_MODE;

uint8_t sensorTypes[] = {CENTER_TS, LEFT_TS, RIGHT_TS};
uint8_t sensorPins[] = {PIN_CENTER_TS, PIN_LEFT_TS, PIN_RIGHT_TS};
const char *SENSOR_NAMES[] = { "Center", "Left", "Right"};

Button2 tsCenter, tsLeft, tsRight;
Button2 TouchSensors[] = {tsCenter, tsLeft, tsRight};

uint8_t lastSensorControlled = CENTER_TS;

bool buttonLongPressed[] = {false, false, false};

void clickHandler(Button2 &btn)
{   
    DEBUG_OK("Short Press %s Button", SENSOR_NAMES[btn.getID()]);
    switch (btn.getID())
    {
    case PIN_CENTER_TS:
       
        MWST_ToggleStripState(STRIP_CENTER);
        lastSensorControlled = STRIP_CENTER;
        break;
    case PIN_LEFT_TS:
        MWST_ToggleStripState(STRIP_LEFT);
        lastSensorControlled = STRIP_LEFT;
        break;
    case PIN_RIGHT_TS:
        MWST_ToggleStripState(STRIP_RIGHT);
        lastSensorControlled = STRIP_RIGHT;
        break;
    default:
        DEBUG_ERROR("Unknown button ID %d", btn.getID());
        break;
    }
}

void longClickDetectedHandler(Button2 &btn)
{
    DEBUG_OK("Long Press %s Button Started", SENSOR_NAMES[btn.getID()]);
    switch (btn.getID())
    {
    case PIN_CENTER_TS:
        // Ignore for center sensor
        //buttonLongPressed[CENTER_TS] = true;
        break;
    case PIN_LEFT_TS:
        buttonLongPressed[LEFT_TS] = true;
        break;
    case PIN_RIGHT_TS:
        buttonLongPressed[RIGHT_TS] = true;
        break;
    default:
    DEBUG_ERROR("Unknown button ID %d", btn.getID());
        break;
    }    
}

void longClickHandler(Button2 &btn)
{
    DEBUG_OK("Long Press %s Button Ended", SENSOR_NAMES[btn.getID()]);
    
    switch (btn.getID())
    {        
    case PIN_CENTER_TS:
        // Ignore for center sensor
        //buttonLongPressed[CENTER_TS] = false;
        break;
    case PIN_LEFT_TS:
        buttonLongPressed[LEFT_TS] = false;
        MWST_ToggleIncreaseBrightness(STRIP_LEFT);
        break;
    case PIN_RIGHT_TS:
        buttonLongPressed[RIGHT_TS] = false;
        MWST_ToggleIncreaseBrightness(STRIP_RIGHT);
        break;
    default:
        DEBUG_ERROR("Unknown button ID %d", btn.getID());
        break;        
    }

    
}

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}

void HMIM_Initialize()
{
    // Initialize Touch sensors
    for (uint8_t sensorType = 0; sensorType < (sizeof(sensorTypes) / sizeof(sensorTypes[0])); sensorType++)
    {
         
        TouchSensors[sensorType] = Button2(sensorPins[sensorType], 0, CAPACITIVE_INPUT, CAPACITIVE_INPUT);
        TouchSensors[sensorType].setID(sensorPins[sensorType]);
        TouchSensors[sensorType].setDebounceTime(DEBOUNCE_TIME);
        TouchSensors[sensorType].setClickHandler(clickHandler);
        TouchSensors[sensorType].setLongClickDetectedHandler(longClickDetectedHandler);
        TouchSensors[sensorType].setLongClickHandler(longClickHandler);        
        TouchSensors[sensorType].setLongClickTime(LONG_CLICK_TIME);
        
    }

    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);

    bool circleValues = false;
    rotaryEncoder.setBoundaries(0, ROTARY_ENCODER_MAX_VALUE_BRIGHTNESS, circleValues);
    rotaryEncoder.setEncoderValue(MWST_GetCurrentBrightness(STRIP_CENTER));
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
                rotaryEncoder.setEncoderValue(MWST_GetColorIndex(lastSensorControlled));
            }
            else
            {
                rotaryMode = ROTARY_BRIGHTNESS_MODE;
                rotaryEncoder.setBoundaries(0, ROTARY_ENCODER_MAX_VALUE_BRIGHTNESS, false);
                rotaryEncoder.setEncoderValue(MWST_GetCurrentBrightness(lastSensorControlled));
            }
        }
    }
    if (rotaryEncoder.encoderChanged())
    {
        if (rotaryMode == ROTARY_BRIGHTNESS_MODE)
        {           
                uint8_t brightness = map(rotaryEncoder.readEncoder(), 0, ROTARY_ENCODER_MAX_VALUE_BRIGHTNESS, 0, 255);
                MWST_SetBrightness(STRIP_CENTER, brightness);
                MWST_SetBrightness(STRIP_LEFT, brightness);
                MWST_SetBrightness(STRIP_RIGHT, brightness);            
        }
        else
        {
            if (rotaryEncoder.readEncoder() < (ROTARY_ENCODER_MAX_VALUE_COLOR / 2))
            {
                float hue = rotaryEncoder.readEncoder() / (ROTARY_ENCODER_MAX_VALUE_COLOR / 2.0);
                MWST_SetStripColor(STRIP_CENTER, RgbwColor(HsbColor(hue, 0.6f, 1.0f)));
                MWST_SetStripColor(STRIP_LEFT, RgbwColor(HsbColor(hue, 0.6f, 1.0f)));
                MWST_SetStripColor(STRIP_RIGHT, RgbwColor(HsbColor(hue, 0.6f, 1.0f)));
            }
            else
            {
                uint16_t encoder_value = rotaryEncoder.readEncoder();
                MWST_SetStripColor(STRIP_CENTER, RgbwColor(0, 0, map(encoder_value - (ROTARY_ENCODER_MAX_VALUE_COLOR / 2), (ROTARY_ENCODER_MAX_VALUE_COLOR / 2),0 , 0, 255), 255));
                MWST_SetStripColor(STRIP_LEFT, RgbwColor(0, 0, map(encoder_value - (ROTARY_ENCODER_MAX_VALUE_COLOR / 2), 0, 0, (ROTARY_ENCODER_MAX_VALUE_COLOR / 2), 255), 255));
                MWST_SetStripColor(STRIP_RIGHT, RgbwColor(0, 0, map(encoder_value - (ROTARY_ENCODER_MAX_VALUE_COLOR / 2), 0, 0, (ROTARY_ENCODER_MAX_VALUE_COLOR / 2), 255), 255));
            }
        }
    }
}

void HMIN_ProcessHMI()
{
    for (uint8_t sensorType = 0; sensorType < (sizeof(sensorTypes) / sizeof(sensorTypes[0])); sensorType++)
    {
        TouchSensors[sensorType].loop();
        if (buttonLongPressed[sensorType])
        {
            MWST_IncreaseStripIlumination(sensorType+1, 1);
        }
    }
    MWIH_ReadRotaryEncoder();
}
