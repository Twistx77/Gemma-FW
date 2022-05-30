
#include "MW_InputHandler.h"
#include "MW_Strip.h"
#include "Button2.h"
#include "AiEsp32RotaryEncoder.h"

#define ROTARY_ENCODER_A_PIN 37
#define ROTARY_ENCODER_B_PIN 10
#define ROTARY_ENCODER_BUTTON_PIN 11

#define ROTARY_ENCODER_STEPS 4

#define ROTARY_BRIGHTNESS_MODE 0
#define ROTARY_COLOR_MODE 1
#define ROTARY_ENCODER_MAX_VALUE 1000
#define ROTARY_ENCODER_ACCELEARTION 250

Button2 mainSensor, leftSensor, rightSensor;

Button2 Sensors[] = {mainSensor, leftSensor, rightSensor};
bool increaseBrightness = true;

bool sensorsLongPressed[] = {false, false, false};

// uint32_t mainSensorFlags, leftSensorFlags, rightSensorFlags;
uint32_t sensorEventFlags[3] = {0, 0, 0}; // = {&mainSensorFlags, &leftSensorFlags, &rightSensorFlags};

uint32_t enabledSensors = 0;

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);
uint8_t rotaryMode = ROTARY_BRIGHTNESS_MODE;

void mainSensorPressHandler(Button2 &btn);
void mainSensorReleasedHandler(Button2 &btn);
void mainSensorSingleTapHandler(Button2 &btn);
void mainSensorDoubleTapHandler(Button2 &btn);
void mainSensorTripleTapHandler(Button2 &btn);
void mainSensorLongPressDetectedHandler(Button2 &btn);
void mainSensorLongPressHandler(Button2 &btn);
void leftSensorPressHandler(Button2 &btn);
void leftSensorReleasedHandler(Button2 &btn);
void leftSensorSingleTapHandler(Button2 &btn);
void leftSensorDoubleTapHandler(Button2 &btn);
void leftSensorTripleTapHandler(Button2 &btn);
void leftSensorLongPressDetectedHandler(Button2 &btn);
void leftSensorLongPressHandler(Button2 &btn);
void rightSensorPressHandler(Button2 &btn);
void rightSensorReleasedHandler(Button2 &btn);
void rightSensorSingleTapHandler(Button2 &btn);
void rightSensorDoubleTapHandler(Button2 &btn);
void rightSensorTripleTapHandler(Button2 &btn);
void rightSensorLongPressDetectedHandler(Button2 &btn);
void rightSensorLongPressHandler(Button2 &btn);

typedef void (*EventHandlerPointer)(Button2 &btn);
EventHandlerPointer pressHandlersPointers[] = {mainSensorPressHandler, leftSensorPressHandler, rightSensorPressHandler};
EventHandlerPointer releasedHandlersPointers[] = {mainSensorReleasedHandler, leftSensorReleasedHandler, rightSensorReleasedHandler};
EventHandlerPointer singleTapHandlersPointers[] = {mainSensorSingleTapHandler, leftSensorSingleTapHandler, rightSensorSingleTapHandler};
EventHandlerPointer doubleTapHandlersPointers[] = {mainSensorDoubleTapHandler, leftSensorDoubleTapHandler, rightSensorDoubleTapHandler};
EventHandlerPointer tripleTapHandlersPointers[] = {mainSensorTripleTapHandler, leftSensorTripleTapHandler, rightSensorTripleTapHandler};
EventHandlerPointer longPressDetectedHandlersPointers[] = {mainSensorLongPressDetectedHandler, leftSensorLongPressDetectedHandler, rightSensorLongPressDetectedHandler};
EventHandlerPointer longPressHandlersPointers[] = {mainSensorLongPressHandler, leftSensorLongPressHandler, rightSensorLongPressHandler};

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}

// Main Sensor
void mainSensorPressHandler(Button2 &btn)
{
    sensorEventFlags[MWIH_MAIN_SENSOR] |= MWIH_PRESSED;
}
void mainSensorReleasedHandler(Button2 &btn)
{
    sensorEventFlags[MWIH_MAIN_SENSOR] |= MWIH_RELEASED;
}
void mainSensorSingleTapHandler(Button2 &btn)
{
    sensorEventFlags[MWIH_MAIN_SENSOR] |= MWIH_SINGLE_TAP;
}

void mainSensorDoubleTapHandler(Button2 &btn)
{
    sensorEventFlags[MWIH_MAIN_SENSOR] |= MWIH_DOUBLE_TAP;
}

void mainSensorTripleTapHandler(Button2 &btn)
{
    sensorEventFlags[MWIH_MAIN_SENSOR] |= MWIH_TRIPLE_TAP;
}

void mainSensorLongPressDetectedHandler(Button2 &btn)
{
    sensorsLongPressed[MWIH_MAIN_SENSOR] = true;
}

void mainSensorLongPressHandler(Button2 &btn)
{
    sensorsLongPressed[MWIH_MAIN_SENSOR] = false;
}

// Left Sensor
void leftSensorPressHandler(Button2 &btn)
{
    sensorEventFlags[MWIH_LEFT_SENSOR] |= MWIH_PRESSED;
    Serial.write(1);
}

void leftSensorReleasedHandler(Button2 &btn)
{
    sensorEventFlags[MWIH_LEFT_SENSOR] |= MWIH_RELEASED;
    Serial.write(2);
}
void leftSensorSingleTapHandler(Button2 &btn)
{
    sensorEventFlags[MWIH_LEFT_SENSOR] |= MWIH_SINGLE_TAP;
    Serial.write(3);
}

void leftSensorDoubleTapHandler(Button2 &btn)
{
    sensorEventFlags[MWIH_LEFT_SENSOR] |= MWIH_DOUBLE_TAP;
}

void leftSensorTripleTapHandler(Button2 &btn)
{
    sensorEventFlags[MWIH_LEFT_SENSOR] |= MWIH_TRIPLE_TAP;
}

void leftSensorLongPressDetectedHandler(Button2 &btn)
{
    sensorsLongPressed[MWIH_LEFT_SENSOR] = true;
    if (increaseBrightness)
    {
        increaseBrightness = false;
    }
    else
    {
        increaseBrightness = true;
    }
    Serial.write(4);
}

void leftSensorLongPressHandler(Button2 &btn)
{
    sensorsLongPressed[MWIH_LEFT_SENSOR] = false;
    Serial.write(5);
}

// Right Sensor
void rightSensorPressHandler(Button2 &btn)
{
    sensorEventFlags[MWIH_RIGHT_SENSOR] |= MWIH_PRESSED;
}

void rightSensorReleasedHandler(Button2 &btn)
{
    sensorEventFlags[MWIH_RIGHT_SENSOR] |= MWIH_RELEASED;
}
void rightSensorSingleTapHandler(Button2 &btn)
{
    sensorEventFlags[MWIH_RIGHT_SENSOR] |= MWIH_SINGLE_TAP;
}

void rightSensorDoubleTapHandler(Button2 &btn)
{
    sensorEventFlags[MWIH_RIGHT_SENSOR] |= MWIH_DOUBLE_TAP;
}

void rightSensorTripleTapHandler(Button2 &btn)
{
    sensorEventFlags[MWIH_RIGHT_SENSOR] |= MWIH_TRIPLE_TAP;
}

void rightSensorLongPressDetectedHandler(Button2 &btn)
{
    sensorsLongPressed[MWIH_RIGHT_SENSOR] = true;
    if (increaseBrightness)
    {
        increaseBrightness = false;
    }
    else
    {
        increaseBrightness = true;
    }
}

void rightSensorLongPressHandler(Button2 &btn)
{
    sensorsLongPressed[MWIH_RIGHT_SENSOR] = false;
}

void MWIH_EnableInputSensor(uint8_t sensorType, uint8_t pin)
{
    enabledSensors |= 0x01 << sensorType;

    Sensors[sensorType] = Button2(pin, 0, true, true);

    // Sensors[sensorType].setReleasedHandler(releasedHandlersPointers[sensorType]);
    Sensors[sensorType].setClickHandler(singleTapHandlersPointers[sensorType]);
    Sensors[sensorType].setLongClickDetectedHandler(longPressDetectedHandlersPointers[sensorType]);
    Sensors[sensorType].setLongClickHandler(longPressHandlersPointers[sensorType]);
    // Sensors[sensorType].setDoubleClickHandler(doubleTapHandlersPointers[sensorType]);
    // Sensors[sensorType].setTripleClickHandler(tripleTapHandlersPointers[sensorType]);

    Sensors[sensorType].setDebounceTime(1);
    Sensors[sensorType].setLongClickTime(400);

    Serial.println(Sensors[sensorType].getDebounceTime());
    Serial.println(Sensors[sensorType].getLongClickTime());
    Serial.println(Sensors[sensorType].getDoubleClickTime());

    sensorEventFlags[sensorType] = 0; // Clear flags
}

void MWIH_InitializeRotaryEncoder()
{
    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);

    bool circleValues = false;
    rotaryEncoder.setBoundaries(0, ROTARY_ENCODER_MAX_VALUE, circleValues);
    rotaryEncoder.setAcceleration(ROTARY_ENCODER_ACCELEARTION);

    rotaryMode = ROTARY_BRIGHTNESS_MODE;
}

void MWIH_RunInputHandler()
{

    for (uint8_t sensor = 0; sensor < MWIH_AVAILABLE_SENSORS; sensor++)
    {
        if (enabledSensors & (0x01 << sensor))
        {
            Sensors[sensor].loop();
        }
    }

    MWIH_ReadRotary();
}

uint32_t MWIH_GetEvent(uint8_t sensor)
{
    uint32_t temp;
    if (sensorsLongPressed[sensor] == true)
    {
        sensorEventFlags[sensor] |= MWIH_SINGLE_LONG_TAP;
    }
    temp = sensorEventFlags[sensor];
    sensorEventFlags[sensor] = 0;
    return temp;
}

void MWIH_ReadRotary()
{

    if (rotaryEncoder.isEncoderButtonClicked())
    {
        static unsigned long lastTimePressed = 0;
        // ignore multiple press in that time milliseconds
        if (millis() - lastTimePressed > 500)
        {
            lastTimePressed = millis();
            // Serial.println("Button Clicked");
            if (rotaryMode == ROTARY_BRIGHTNESS_MODE)
            {
                rotaryMode == ROTARY_COLOR_MODE;
            }
            else
            {
                rotaryMode == ROTARY_BRIGHTNESS_MODE;
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

        // Serial.print("Value: ");
        // Serial.println(rotaryEncoder.readEncoder());
    }
}
