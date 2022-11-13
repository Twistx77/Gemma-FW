#ifndef DEFAULT_CONFIG_H
#define DEFAULT_CONFIG_H
 
#define FW_MAJOR 0
#define FW_MINOR 5
#define FW_PATCH 6

//#define BT_DEBUG 

#ifdef BT_DEBUG
#include "BluetoothSerial.h"
static BluetoothSerial SerialBT;
#endif

#define MAX_NUMBER_OF_LEDS 105

#define CAPTOUCH_THLD_BOOT 6

#define DEFAULT_SATURATION 0.6f

#define ROTARY_ENCODER_MAX_VALUE_COLOR 512.0      // TODO: REPLACE CONFIG MANAGER

#define NL_BRIGHTNESS_CHANGE_DELAY_MS 15 // Delay it takes between steps of brigthness for the night light



///// ENCODER /////
#define ROTARY_ENCODER_A_PIN 25      // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_B_PIN 23      // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_BUTTON_PIN 26 // TODO: REPLACE CONFIG MANAGER

#define ROTARY_ENCODER_STEPS 4 // TODO: REPLACE CONFIG MANAGER

#define ROTARY_BRIGHTNESS_MODE 0                  // TODO: REPLACE CONFIG MANAGER
#define ROTARY_COLOR_MODE 1                       // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_MAX_VALUE_BRIGHTNESS 255.0 // TODO: REPLACE CONFIG MANAGER
#define ROTARY_ENCODER_ACCELERATION 250           // TODO: REPLACE CONFIG MANAGER

////

//#define TEST_BOARD

#if defined(TEST_BOARD)
#define LEDS_NL 3
#define LEDS_STRIP 30
#define PIN_STRIP 16

#define PIN_CENTER_TS 27 // TODO: REPLACE CONFIG MANAGER
#define PIN_LEFT_TS 4    // TODO: REPLACE CONFIG MANAGER
#define PIN_RIGHT_TS 2   // TODO: REPLACE CONFIG MANAGER

#define PIN_LED 2

#define CAPACITIVE_INPUT true

#define MAX_BRIGHTNESS 55


#else
#define LEDS_NL 3
#define LEDS_STRIP 106
#define PIN_STRIP 16

#define PIN_CENTER_TS 27 // TODO: REPLACE CONFIG MANAGER
#define PIN_LEFT_TS 2    // TODO: REPLACE CONFIG MANAGER
#define PIN_RIGHT_TS 4   // TODO: REPLACE CONFIG MANAGER

#define PIN_LED 19

#define CAPACITIVE_INPUT true

#define MAX_BRIGHTNESS 255
#endif

#define ACTIVE_LOW CAPACITIVE_INPUT


#endif