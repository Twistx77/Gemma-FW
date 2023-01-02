// AlarmsManager.cpp
#include "AlarmsManager.h"
#include "DefaultConfig.h"
#include <Arduino.h>

static void IRAM_ATTR rtc_int_isr()
{
}

// Initialize the AlarmsManager
void AlarmsManager::initialize()
{
    rtc.initialize();
    rtc.timerSet(rtc.TIMER_CLOCK_1PER60HZ, 1, true, true); // Enable RTC timer with interrupt pulse every minute
    pinMode(PIN_RTC_INT, INPUT);
    attachInterrupt(PIN_RTC_INT, rtc_int_isr, RISING);
}
