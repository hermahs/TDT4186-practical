#ifndef ALARM_H_
#define ALARM_H_

#include <time.h>

typedef struct {
    time_t time;
} Alarm;

#define MAX_ALARMS 20

#endif // ALARM_H_
