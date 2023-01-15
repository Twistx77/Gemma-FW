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

typedef struct AlarmParameters
{
    uint8_t enabled;
    uint8_t hours;
    uint8_t minutes;
    uint8_t weekdays;
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
    ALARMS_MAX = 6
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