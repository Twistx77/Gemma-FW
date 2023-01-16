#ifndef _H_HMIM_HMIMANAGER
#define _H_HMIM_HMIMANAGER

#include <stdint.h>

typedef enum
{
    TS_CENTER = 0,
    TS_LEFT = 1,
    TS_RIGHT= 2,
    MAX_TOUCH_SENSORS
} TouchSensor_t;

void HMIM_Initialize();
void HMIM_ProcessHMI();

#endif