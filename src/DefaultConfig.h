#ifndef DEFAULT_CONFIG_H
#define DEFAULT_CONFIG_H
 
#define FW_MAJOR 0
#define FW_MINOR 5
#define FW_PATCH 4

#define MAX_NUMBER_OF_LEDS 105

#define CAPTOUCH_THLD_BOOT 6


//#define TEST_BOARD


#if defined(TEST_BOARD)
#define LEDS_NL 5
#define LEDS_STRIP 90
#define PIN_STRIP 16

#define PIN_CENTER_TS 27 // TODO: REPLACE CONFIG MANAGER
#define PIN_LEFT_TS 4    // TODO: REPLACE CONFIG MANAGER
#define PIN_RIGHT_TS 2   // TODO: REPLACE CONFIG MANAGER

#define PIN_LED 2

#define CAPACITIVE_INPUT true

#else
#define LEDS_NL 3
#define LEDS_STRIP 106
#define PIN_STRIP 16

#define PIN_CENTER_TS 27 // TODO: REPLACE CONFIG MANAGER
#define PIN_LEFT_TS 2    // TODO: REPLACE CONFIG MANAGER
#define PIN_RIGHT_TS 4   // TODO: REPLACE CONFIG MANAGER

#define PIN_LED 19

#define CAPACITIVE_INPUT true
#endif







#endif