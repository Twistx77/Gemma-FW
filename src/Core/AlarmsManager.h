#ifndef ALARMS_MANAGER_H
#define ALARMS_MANAGER_H

#include <stdint.h>
#include "../Drivers/PCF85063A.h"

enum Weekdays
{
    MONDAY = 0x01,
    TUESDAY = 0x02,
    WEDNESDAY = 0x04,
    THURSDAY = 0x08,
    FRIDAY = 0x10,
    SATURDAY = 0x20,
    SUNDAY = 0x40
};

typedef struct AlarmTimeAndDate
{
    uint8_t hours;
    uint8_t minutes;
    uint8_t weekday;

} AlarmTimeAndDate;

typedef struct AlarmParameters
{
    uint8_t enabled;
    AlarmTimeAndDate timeAndDateOn;
    AlarmTimeAndDate timeAndDateOff;
    uint8_t maxBrightness;
    uint16_t secondsToFullBrightness;
    uint32_t color;
} AlarmParameters;

typedef struct TimeAndDate
{
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t weekday;
} TimeAndDate;

enum Alarm
{
    ALARM_1 = 0,
    ALARM_2 = 1,
    ALARM_3 = 3,
    ALARM_4 = 4,
    ALARM_5 = 5,
    ALARM_6 = 6,
    ALARM_7 = 7,
    ALARM_8 = 8,
    ALARM_9 = 9,
    ALARM_10 = 10,
    ALARMS_MAX = 11
};

class AlarmsManager
{
public:
    void initialize();
    void setTimeAndDate(TimeAndDate timeAndDate);
    TimeAndDate getTimeAndDate();
    void setAlarm(Alarm alarm, AlarmParameters parameters);
    AlarmParameters getAlarm(Alarm alarm);
    bool checkAlarms();

private:
    PCF85063A rtc;
    AlarmParameters alarms[ALARMS_MAX - 1];
};

#endif // ALARMS_MANAGER_H