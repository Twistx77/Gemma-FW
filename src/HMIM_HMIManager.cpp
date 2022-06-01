#include "HMIM_HMIManager.h"
#include "MW_Strip.h"
#include "Button2.h"
#include "AiEsp32RotaryEncoder.h"

#define CENTER_TS 0
#define LEFT_TS 1
#define RIGHT_TS 2

#define PIN_CENTER_TS 12 // TODO: REPLACE CONFIG MANAGER
#define PIN_LEFT_TS 13   // TODO: REPLACE CONFIG MANAGER
#define PIN_RIGHT_TS 14  // TODO: REPLACE CONFIG MANAGER

#define DEBOUNCE_TIME 1     // TODO: REPLACE CONFIG MANAGER
#define LONG_CLICK_TIME 400 // TODO: REPLACE CONFIG MANAGER

#define ROTARY_ENCODER_A_PIN 37      // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_B_PIN 10      // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_BUTTON_PIN 11 // TODO: REPLACE CONFIG MANAGER

#define ROTARY_ENCODER_STEPS 4 // TODO: REPLACE CONFIG MANAGER

#define ROTARY_BRIGHTNESS_MODE 0        // TODO: REPLACE CONFIG MANAGER
#define ROTARY_COLOR_MODE 1             // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_MAX_VALUE 1000   // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_ACCELEARTION 250 // TODO: REPLACE CONFIG MANAGER

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);

uint8_t rotaryMode = ROTARY_BRIGHTNESS_MODE; 

uint8_t sensorTypes[] = {CENTER_TS, LEFT_TS, RIGHT_TS};
uint8_t sensorPins[] = {PIN_CENTER_TS, PIN_LEFT_TS, PIN_RIGHT_TS};

Button2 tsCenter, tsLeft, tsRight;
Button2 TouchSensors[] = {tsCenter, tsLeft, tsRight};

void clickHandler(Button2 &btn)
{
    if (btn == tsCenter)
    {
    }
    else if (btn == tsLeft)
    {
    }
    else if (btn == tsRight)
    {
    }
}

void longClickDetectedHandler(Button2 &btn)
{
}

void longClickHandler(Button2 &btn)
{
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
        TouchSensors[sensorType].setDebounceTime(DEBOUNCE_TIME);
        TouchSensors[sensorType].setLongClickTime(LONG_CLICK_TIME);
        TouchSensors[sensorType].setClickHandler(clickHandler);
        TouchSensors[sensorType].setLongClickDetectedHandler(longClickDetectedHandler);
        TouchSensors[sensorType].setLongClickHandler(longClickHandler);
    }

    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);

    bool circleValues = false;
    rotaryEncoder.setBoundaries(0, ROTARY_ENCODER_MAX_VALUE, circleValues);
    rotaryEncoder.setAcceleration(ROTARY_ENCODER_ACCELEARTION);

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
            }
            else
            {
                rotaryMode = ROTARY_BRIGHTNESS_MODE;
            }
        }
    }
    if (rotaryEncoder.encoderChanged())
    {
        if (rotaryMode == ROTARY_BRIGHTNESS_MODE)
        {
            uint8_t brightness = map(rotaryEncoder.readEncoder(), 0, ROTARY_ENCODER_MAX_VALUE, 0, 255);
            MWST_SetBrightness(STRIP_LEFT, brightness);
        }
        else
        {

            if (rotaryEncoder.readEncoder() < (ROTARY_ENCODER_MAX_VALUE / 2))
            {
                float hue = rotaryEncoder.readEncoder() / 500.0;
                MWST_SetStripColor(STRIP_LEFT, RgbwColor(HsbColor(hue, 0.8f, 1.0f)));
            }
            else
            {

                MWST_SetStripColor(STRIP_LEFT, RgbwColor(0, 0, map(rotaryEncoder.readEncoder() - 500, 0, 500, 0, 255), 255));
            }
        }
    }
}

void HMIN_ProcessHMI()
{
    for (uint8_t sensorType = 0; sensorType < (sizeof(sensorTypes) / sizeof(sensorTypes[0])); sensorType++)
    {

        TouchSensors[sensorType].loop();
    }
    MWIH_ReadRotaryEncoder();
}
