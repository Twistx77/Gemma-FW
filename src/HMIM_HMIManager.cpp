#include "HMIM_HMIManager.h"
#include "MW_Strip.h"
#include "Button2.h"
#include "AiEsp32RotaryEncoder.h"

#define CENTER_TS 0
#define LEFT_TS 1
#define RIGHT_TS 2

#define PIN_CENTER_TS 27 // TODO: REPLACE CONFIG MANAGER
#define PIN_LEFT_TS 2    // TODO: REPLACE CONFIG MANAGER
#define PIN_RIGHT_TS 4   // TODO: REPLACE CONFIG MANAGER

#define DEBOUNCE_TIME 1     // TODO: REPLACE CONFIG MANAGER
#define LONG_CLICK_TIME 500 // TODO: REPLACE CONFIG MANAGER

#define ROTARY_ENCODER_A_PIN 23      // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_B_PIN 25      // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_BUTTON_PIN 26 // TODO: REPLACE CONFIG MANAGER

#define ROTARY_ENCODER_STEPS 4 // TODO: REPLACE CONFIG MANAGER

#define ROTARY_BRIGHTNESS_MODE 0                  // TODO: REPLACE CONFIG MANAGER
#define ROTARY_COLOR_MODE 1                       // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_MAX_VALUE_BRIGHTNESS 255.0 // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_MAX_VALUE_COLOR 512.0     // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_ACCELERATION 250           // TODO: REPLACE CONFIG MANAGER

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);

uint8_t rotaryMode = ROTARY_BRIGHTNESS_MODE;

uint8_t sensorTypes[] = {CENTER_TS, LEFT_TS, RIGHT_TS};
uint8_t sensorPins[] = {PIN_CENTER_TS, PIN_LEFT_TS, PIN_RIGHT_TS};

Button2 tsCenter, tsLeft, tsRight;
Button2 TouchSensors[] = {tsCenter, tsLeft, tsRight};

uint8_t lastSensorControlled = CENTER_TS;

bool buttonLongPressed [] = {false, false, false};

void clickHandler(Button2 &btn)
{
    switch (btn.getID())
    {
    case PIN_CENTER_TS:
        Serial.println("Center TS clicked");
        MWST_ToggleStripState(STRIP_CENTER);
        lastSensorControlled = STRIP_CENTER;
        break;
    case PIN_LEFT_TS:
        Serial.println("Left TS clicked");
        MWST_ToggleStripState(STRIP_LEFT);
        lastSensorControlled = STRIP_LEFT;
        break;
    case PIN_RIGHT_TS:
        Serial.println("Right TS clicked");
    
        MWST_ToggleStripState(STRIP_RIGHT);
        lastSensorControlled = STRIP_RIGHT;
        break;
    default:
        Serial.println("ERROR: UNKNOWN BUTTON ID");
        break;
    }
    if (rotaryMode == ROTARY_BRIGHTNESS_MODE)
    {
        rotaryEncoder.setBoundaries(0, ROTARY_ENCODER_MAX_VALUE_BRIGHTNESS, false);
        rotaryEncoder.setEncoderValue(MWST_GetBrightness(lastSensorControlled));
    }
    else
    {
        rotaryEncoder.setBoundaries(0, ROTARY_ENCODER_MAX_VALUE_COLOR, false);
        rotaryEncoder.setEncoderValue(MWST_GetColorIndex(lastSensorControlled));
    }
    rotaryEncoder.encoderChanged(); //Read encoder after change to avoid being detected in next iteration
}

void longClickDetectedHandler(Button2 &btn)
{
    Serial.println("longClickDetected");
    /*switch (btn.getID())
    {
    case PIN_CENTER_TS:
        buttonLongPressed[CENTER_TS] = true;
        break;
    case PIN_LEFT_TS:
        buttonLongPressed[LEFT_TS] = true;
        break;
    case PIN_RIGHT_TS:
        buttonLongPressed[RIGHT_TS] = true;
        break;
    default:
        Serial.println("ERROR: UNKNOWN BUTTON ID");
        break;
    }
    */
}

void longClickHandler(Button2 &btn)
{
    Serial.println("longClick");
    /*
    switch (btn.getID())
    {   
    case PIN_CENTER_TS:
        buttonLongPressed[CENTER_TS] = false;
        break;
    case PIN_LEFT_TS:
        buttonLongPressed[LEFT_TS] = false;
        break;
    case PIN_RIGHT_TS:
        buttonLongPressed[RIGHT_TS] = false;
        break;
    default:
        Serial.println("ERROR: UNKNOWN BUTTON ID");
        break;
    }*/
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
        TouchSensors[sensorType] = Button2(sensorPins[sensorType], 0, true, true);
        TouchSensors[sensorType].setID(sensorPins[sensorType]);
        TouchSensors[sensorType].setDebounceTime(DEBOUNCE_TIME);
        TouchSensors[sensorType].setClickHandler(clickHandler);
        TouchSensors[sensorType].setLongClickDetectedHandler(longClickDetectedHandler);
        TouchSensors[sensorType].setLongClickHandler(longClickHandler);

        if (sensorType == CENTER_TS) // Remove long click possibility from center sensor
        {
            TouchSensors[sensorType].setLongClickHandler(NULL);
            TouchSensors[sensorType].setLongClickHandler(NULL);
            
            TouchSensors[sensorType].setLongClickTime(LONG_CLICK_TIME*10);
        }else
        {
            
            TouchSensors[sensorType].setLongClickTime(LONG_CLICK_TIME);
        }
        
        
    }

    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);

    bool circleValues = false;
    rotaryEncoder.setBoundaries(0, ROTARY_ENCODER_MAX_VALUE_BRIGHTNESS, circleValues);
    rotaryEncoder.setEncoderValue(MWST_GetBrightness(STRIP_CENTER));
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
                rotaryEncoder.setEncoderValue(MWST_GetBrightness(lastSensorControlled));
            }
            else
            {
                rotaryMode = ROTARY_BRIGHTNESS_MODE;
                rotaryEncoder.setBoundaries(0, ROTARY_ENCODER_MAX_VALUE_BRIGHTNESS, false);
                rotaryEncoder.setEncoderValue(MWST_GetBrightness(lastSensorControlled));
            }
        }
    }
    if (rotaryEncoder.encoderChanged())
    {  
        Serial.println("Encoder changed");

        if (rotaryMode == ROTARY_BRIGHTNESS_MODE)
        {
            uint8_t brightness = map(rotaryEncoder.readEncoder(), 0, ROTARY_ENCODER_MAX_VALUE_BRIGHTNESS, 0, 255);
            MWST_SetBrightness(MWST_GetLastStripActive(), brightness);
        }
        else
        {

            if (rotaryEncoder.readEncoder() < (ROTARY_ENCODER_MAX_VALUE_COLOR / 2))
            {

                float hue = rotaryEncoder.readEncoder() / (ROTARY_ENCODER_MAX_VALUE_COLOR / 2.0);
                Serial.println("Color " + String(hue));
                MWST_SetStripColor(lastSensorControlled, RgbwColor(HsbColor(hue, 0.8f, 1.0f)));
            }
            else
            {
                MWST_SetStripColor(lastSensorControlled, RgbwColor(0, 0, map(rotaryEncoder.readEncoder() - (ROTARY_ENCODER_MAX_VALUE_COLOR / 2), 0, (ROTARY_ENCODER_MAX_VALUE_COLOR / 2), 0, 255), 255));
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
            MWST_IncreaseStripIlumination(sensorType,10);
        }
    }
    MWIH_ReadRotaryEncoder();
}
