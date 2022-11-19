#include "HMIM_HMIManager.h"
#include "MW_Strip.h"
#include "Button2.h"
#include "AiEsp32RotaryEncoder.h"
#include "DefaultConfig.h"
#include "driver/touch_sensor.h"
#include "esp32-hal-touch.h"

//#define DEBUG_THIS_FILE 
#include "PrettyDebug.h"

#define CENTER_TS 0
#define LEFT_TS 1
#define RIGHT_TS 2



#define DEBOUNCE_TIME 1     // TODO: REPLACE CONFIG MANAGER
#define LONG_CLICK_TIME 1000 // TODO: REPLACE CONFIG MANAGER



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
        //buttonLongPressed[CENTER_TS] = false;
        //WST_ToggleIncreaseBrightness(STRIP_CENTER);
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

    
    
    touch_pad_init();    
    
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_0V); 
    // Set measuring time for the Touch Sensor FSM to improve resolution for bigger capacitive surface
    touch_pad_set_meas_time(50, 20000);
    // Touch Sensor Timer initiated
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);  
    touch_pad_filter_start(20);

    // Initialize Touch sensors
    for (uint8_t sensorType = 0; sensorType < (sizeof(sensorTypes) / sizeof(sensorTypes[0])); sensorType++)
    {
        
         
        TouchSensors[sensorType] = Button2(sensorPins[sensorType], 0, CAPACITIVE_INPUT, ACTIVE_LOW);
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
                RgbwColor Color = RgbwColor(0, 0, map(encoder_value - (ROTARY_ENCODER_MAX_VALUE_COLOR / 2), (ROTARY_ENCODER_MAX_VALUE_COLOR / 2),0 , 0, 255), 255); 
                MWST_SetStripColor(STRIP_CENTER, Color);
                MWST_SetStripColor(STRIP_LEFT, Color);
                MWST_SetStripColor(STRIP_RIGHT, Color);
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
