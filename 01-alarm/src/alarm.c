#include "alarm.h"
#include <stdio.h>
#include <time.h>

Alarm alarms[MAX_ALARMS];
int alarm_count;

void add_alarm(time_t target_time) {
    if (alarm_count >= MAX_ALARMS) {
        printf("Alarm list is full!");
        return;
    }

    time_t now;
    time(&now);

    if (target_time <= now) {
        printf("An alarm must be set in the future!");
        return;
    }

    time_t seconds_left = target_time - now;
    printf("Scheduling alarm in %d seconds\n", seconds_left);
}
