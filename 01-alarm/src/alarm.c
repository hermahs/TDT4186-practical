#include "alarm.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <assert.h>
#include "timeutil.h"

// Global list of active variables
// Can contain finished alarms (zombies), until they get cleaned up
Alarm alarms[MAX_ALARMS];
int alarm_count;

static pid_t spawn_alarm(int alarm, time_t sleep_time) {

    pid_t pid = fork();
    if (pid == 0) { // child process
        sleep(sleep_time);
        printf("RING from alarm\n");
        exit(EXIT_SUCCESS); // The child is done now!
    }

    return pid;
}

void add_alarm(time_t target_time) {
    if (alarm_count >= MAX_ALARMS) {
        printf("Alarm list is full!");
        return;
    }

    // Make sure the alarm is acutally in the future
    time_t now = time(NULL);
    if (target_time <= now) {
        printf("An alarm must be set in the future!");
        return;
    }

    // Print the number of seconds left
    time_t seconds_left = target_time - now;
    printf("Scheduling alarm in %d seconds\n", seconds_left);

    // Store the alarm's target time
    // Only used when printing the alarm list
    alarms[alarm_count].time = target_time;
    // Start the alarm frok and save its process id
    alarms[alarm_count].pid = spawn_alarm(alarm_count, seconds_left);
    alarm_count++;
}

void list_alarms() {
    printf("There are %d active alarms:\n", alarm_count);
    for(int i = 0; i < alarm_count; i++)
        printf("Alarm %2d at %s\n", (i+1), time_as_string(alarms[i].time));
}

// Does a linear pass across the alarm list, and removes all alarms where the pid is 0
static void remove_alarms_with_pid0() {

    // this is the array slot we move items to, which will have no holes
    int target = 0;
    // no need to move anything until target is in a hole, and needs to be filled
    while(target < alarm_count && alarms[target].pid) target++;

    // go though the items that come after the hole, and fill in
    for(int from = target; from < alarm_count; from++) {
        if(alarms[from].pid)
            alarms[target++] = alarms[from];
    }

    // the new size now that all holes are filled
    alarm_count = target;
}

// Checks all forks to see if they have finished.
// if so, removes them from the alarm list
void cleanup_zombies() {
    for(int i = 0; i < alarm_count; i++) {
        int wstatus;
        // waitpid returns a positive number if the process has changed
        if(waitpid(alarms[i].pid, &wstatus, WNOHANG) > 0)
            if(WIFEXITED(wstatus) || WIFSIGNALED(wstatus))
                alarms[i].pid = 0; //mark for deletion
    }
    remove_alarms_with_pid0();
}
