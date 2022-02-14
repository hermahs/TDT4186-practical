#ifndef ALARM_H_
#define ALARM_H_

#include <time.h>

typedef struct {
    time_t time;
} Alarm;

#define MAX_ALARMS 20

void add_alarm(time_t target_time);

#endif // ALARM_H_
