#include <Arduino.h>

#include "AlarmsManager.h"
#include "../DefaultConfig.h"
#include "MW_Strip.h"

#define MAX_SECONDS_TO_FULL_BRIGHTNESS 3600


volatile static bool minuteIntFlag;


AlarmParameters alarms[ALARMS_MAX];

// RTC interrupt service routine. This is called every minute
// and allow us to check if the alarm is triggered
static void IRAM_ATTR rtc_int_isr()
{
    minuteIntFlag = true;
}

// Initialize the AlarmsManager
void AlarmsManager::initialize()
{
    // Initialize the flag to false
    minuteIntFlag = false;

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
    if (timeAndDate.hours < 0 || timeAndDate.hours > 23 ||
        timeAndDate.minutes < 0 || timeAndDate.minutes > 59 ||
        timeAndDate.seconds < 0 || timeAndDate.seconds > 59 ||
        timeAndDate.year < 0 || timeAndDate.year > 99 ||
        timeAndDate.month < 1 || timeAndDate.month > 12 ||
        timeAndDate.day < 1 || timeAndDate.day > 31 ||
        timeAndDate.weekday < 1 || timeAndDate.weekday > 127)
        return;

    rtc.setTime(timeAndDate.hours, timeAndDate.minutes, timeAndDate.seconds);
    rtc.setDate(timeAndDate.year, timeAndDate.month, timeAndDate.day, timeAndDate.weekday);
}

// Get the time and date
TimeAndDate AlarmsManager::getTimeAndDate()
{
    TimeAndDate timeAndDate;
    timeAndDate.hours = rtc.getHour();
    timeAndDate.minutes = rtc.getMinute();
    timeAndDate.seconds = rtc.getSecond();
    timeAndDate.year = rtc.getYear();
    timeAndDate.month = rtc.getMonth();
    timeAndDate.day = rtc.getDay();
    timeAndDate.weekday = rtc.getWeekday();

    return timeAndDate;
}

// Set the alarm
void AlarmsManager::setAlarm(Alarm alarm, AlarmParameters parameters)
{
    // Check if alarm is valid
    if (alarm < 0 || alarm > ALARMS_MAX)
        return;

    // Check if parameters are valid:
    // - Enabled: 0 or 1
    // - Alarm Time On : 0 <= hour <= 23, 0 <= minutes <= 59
    // - Weekday : 0 <= weekday <= 127
    // - Alarm Time Off : 0 <= hour <= 23, 0 <= minutes <= 59
    // - Weekday Off : 0 <= weekday <= 127
    // - Max brightness : 0 <= max brightness <= 255
    // - Seconds to Full Brightness : 0 <= seconds to full brightness <= 65535
    if (parameters.enabled < 0 || parameters.enabled > 1 ||
        parameters.timeAndDateOn.hours < 0 || parameters.timeAndDateOn.hours > 23 ||
        parameters.timeAndDateOn.minutes < 0 || parameters.timeAndDateOn.minutes > 59 ||
        parameters.timeAndDateOn.weekday < 0 || parameters.timeAndDateOn.weekday > 127 ||
        parameters.timeAndDateOff.hours < 0 || parameters.timeAndDateOff.hours > 23 ||
        parameters.timeAndDateOff.minutes < 0 || parameters.timeAndDateOff.minutes > 59 ||
        parameters.timeAndDateOff.weekday < 0 || parameters.timeAndDateOff.weekday > 127 ||
        parameters.maxBrightness < 0 || parameters.maxBrightness > 255 ||
        parameters.secondsToFullBrightness < 0 || parameters.secondsToFullBrightness > MAX_SECONDS_TO_FULL_BRIGHTNESS)
        return;

    // Add alarm to the list
    alarms[alarm] = parameters;
}

// Get the alarm
AlarmParameters AlarmsManager::getAlarm(Alarm alarm)
{
    // Check if alarm is valid
    if (alarm < 0 || alarm > ALARMS_MAX)
    {
        AlarmParameters parameters;
        return parameters;
    }

    return alarms[alarm];
}

// Check if the alarm is triggered
bool AlarmsManager::checkAlarms()
{

    // Check if brightness has to be increased for the alarm that is active
    checkBrightnessIncrease();

    if (!minuteIntFlag)
        return false;

    // Reset the flag
    minuteIntFlag = false;

    // Check if the alarm is triggered
    uint8_t hours = rtc.getHour();
    uint8_t minutes = rtc.getMinute();
    uint8_t weekday = rtc.getWeekday();

    // Check if the alarm is triggered
    for (uint8_t alarm = 0; alarm < ALARMS_MAX; alarm++)
    {
        if (alarms[alarm].enabled == 1)
        {            
            if (alarms[alarm].timeAndDateOn.hours == hours &&
            alarms[alarm].timeAndDateOn.minutes == minutes &&
            (alarms[alarm].timeAndDateOn.weekday & (1 << (weekday - 1))) != 0)
            {
                // Alarm ON triggered
                RgbwColor color = RgbwColor(alarms[alarm].color & 0xFF, (alarms[alarm].color >> 8) & 0xFF, (alarms[alarm].color >> 16) & 0xFF, (alarms[alarm].color >> 24) & 0xFF);
                
                secondsToFullBrightness = alarms[alarm].secondsToFullBrightness;

                brightnessIncrement = (float)alarms[alarm].maxBrightness / (float)secondsToFullBrightness;                
                MWST_SetStripColor(STRIP_CENTER, color);                
                MWST_SetBrightness(STRIP_CENTER, brightnessIncrement);                               

                return true; // Alarm triggered
            }
            else
            {
                if (alarms[alarm].timeAndDateOff.hours == hours &&
                    alarms[alarm].timeAndDateOff.minutes == minutes &&
                    (alarms[alarm].timeAndDateOff.weekday & (1 << (weekday - 1))) != 0)
                {
                    // Alarm OFF triggered         
                    MWST_SetStripState(STRIP_CENTER, MWST_DISABLED, EFFECT_FADE);
                    return true; // Alarm triggered
                }
            }
        }
    }
    return false; // Alarm not triggered
}

// Check if brightness has to be increased
void AlarmsManager::checkBrightnessIncrease()
{
    if  (secondsToFullBrightness == 0)
        return;

    secondsToFullBrightness--;
    
    if (secondsToFullBrightness == 0)
    {
        // Set brightness to max alarm brightness
        MWST_SetBrightness(STRIP_CENTER, alarmBrightness);
        return;
    }
    else 
    {
    // Increase brightness by brightness increment 
    MWST_SetBrightness(STRIP_CENTER, MWST_GetCurrentBrightness(STRIP_CENTER)+ brightnessIncrement);
    } 
}

