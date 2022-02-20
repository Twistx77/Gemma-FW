#include <stdint.h>

#ifndef _H_MW_INPUTHANDLER
#define _H_MW_INPUTHANDLER

#define MWIH_AVAILABLE_SENSORS 3

#define MWIH_MAIN_SENSOR 0
#define MWIH_LEFT_SENSOR 1
#define MWIH_RIGHT_SENSOR 2



#define MWIH_NO_EVENT 0x00
#define MWIH_PRESSED 0x01
#define MWIH_RELEASED 0x02
#define MWIH_SINGLE_TAP 0x04
#define MWIH_DOUBLE_TAP 0x08
#define MWIH_TRIPLE_TAP 0x10
#define MWIH_SINGLE_LONG_TAP 0x20
#define MWIH_DOUBLE_LONG_TAP 0x40
#define MWIH_TRIPLE_LONG_TAP 0x80


typedef uint32_t MIWH_TypeEventFlags;

void MWIH_EnableInputSensor(uint8_t sensorType, uint8_t pin);
void MWIH_RunInputHandler();
uint32_t MWIH_GetEvent(uint8_t sensor);
#endif
