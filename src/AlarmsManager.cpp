// AlarmsManager.cpp
#include "AlarmsManager.h"
#include "DefaultConfig.h"
#include <Arduino.h>

// RTC interrupt service routine. This is called every minute
// and allow us to check if the alarm is triggered
static void IRAM_ATTR rtc_int_isr()
{
   minuteIntFlag = true;
}

// Initialize the AlarmsManager
void AlarmsManager::initialize()
{
    // Intitialize alarms to 0
    for (int i = 0; i < MAX_ALARMS; i++)
    {
        alarms[i].enabled = 0;
        alarms[i].hour = 0;
        alarms[i].minutes = 0;
        alarms[i].weekdays = 0;
    }
    // Initialize the RTC
    rtc.initialize();

    // Enable RTC timer with interrupt pulse every minute and enable timer interrupt
    rtc.timerSet(rtc.TIMER_CLOCK_1PER60HZ, 1, true, true);

    // Enable RTC Int pin interrrupt to receive the timer interrupt every minute,
    // which allow us to check if the alarm is triggered
    pinMode(PIN_RTC_INT, INPUT);
    attachInterrupt(PIN_RTC_INT, rtc_int_isr, RISING);
}

// Set the time and date
void AlarmsManager::setTimeAndDate(TimeAndDate timeAndDate)
{
    // Check if time and dates are valid:
    // - Time: 0 <= hour <= 23, 0 <= minutes <= 59, 0 <= seconds <= 59
    // - Date: 0 <= year <= 99, 1 <= month <= 12, 1 <= day <= 31, 1 <= weekday <= 7
    if (timeAndDate.hour < 0 || timeAndDate.hour > 23 ||
        timeAndDate.minutes < 0 || timeAndDate.minutes > 59 ||
        timeAndDate.seconds < 0 || timeAndDate.seconds > 59 ||
        timeAndDate.year < 0 || timeAndDate.year > 99 ||
        timeAndDate.month < 1 || timeAndDate.month > 12 ||
        timeAndDate.day < 1 || timeAndDate.day > 31 ||
        timeAndDate.weekday < 1 || timeAndDate.weekday > 7)
        return;

    rtc.setTime(timeAndDate.hour, timeAndDate.minutes, timeAndDate.seconds);
    rtc.setDate(timeAndDate.year, timeAndDate.month, timeAndDate.day, timeAndDate.weekday);
}

// Set the alarm
void AlarmsManager::setAlarm(Alarm alarm, AlarmParameters parameters)
{
    // Check if alarm is valid
    if (alarm < 0 || alarm > this.MAX_ALARM)
        return;

    // Check if parameters are valid:
    // - Time: 0 <= hour <= 23, 0 <= minutes <= 59, 0 <= seconds <= 59
    // - Date: 0 <= year <= 99, 1 <= month <= 12, 1 <= day <= 31, 1 <= weekday <= 7
    if (parameters.hour < 0 || parameters.hour > 23 ||
        parameters.minutes < 0 || parameters.minutes > 59 ||
        parameters.seconds < 0 || parameters.seconds > 59 ||
        parameters.year < 0 || parameters.year > 99 ||
        parameters.month < 1 || parameters.month > 12 ||
        parameters.day < 1 || parameters.day > 31 ||
        parameters.weekday < 1 || parameters.weekday > 7)
        return;

    // Add alarm to the list
    alarms[alarm] = parameters;
}

// Check if the alarm is triggered
void AlarmsManager::checkAlarms()
{
    if (!minuteIntFlag)
        return;
    
    // Reset the flag
    minuteIntFlag = false;

    // Check if the alarm is triggered
    uint8_t hours = rtc.getHour();
    uint8_t minutes = rtc.getMinute();
    uint8_t weekday = rtc.getWeekday();

    // Check if the alarm is triggered
    for (int i = 0; i < MAX_ALARMS; i++)
    {
        if (alarms[i].enabled == 1 &&
            alarms[i].hour == hours &&
            alarms[i].minutes == minutes &&
            alarms[i].weekdays & (1 << (weekday - 1)))
        {
            // Alarm triggered
        }
    }
}
