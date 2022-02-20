#ifndef ALARM_H_
#define ALARM_H_

#include <time.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct {
    time_t time;
    pid_t pid;
    int sound_number;
    bool has_been_listed;
} Alarm;

#define MAX_ALARMS 20
#define SOUND_COUNT 3

void add_alarm(time_t target_time, int sound_number);

void list_alarms();

void cancel_alarm(int alarm_number);

void cancel_all_alarms();

void cleanup_zombies();

#endif // ALARM_H_
