#include "alarm.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include "timeutil.h"

#define MAX(a, b) ((a)>(b)?(a):(b))

// Global list of active alarms.
// Can contain finished alarms (zombies), until they get cleaned up.
// Once an alarm gets cleaned up its pid gets set to 0, ready to be overridden.
// An extra sentinal 0-alarm is added past the end, to allow overshooting.
Alarm alarms[MAX_ALARMS+1];

char* sounds[SOUND_COUNT] = {
    "audio/windows-xp-startup.mp3",
    "audio/Alarm-clock-bell-ringing-sound-effect.mp3",
    "audio/lobby-classic-game-halloween.mp3"
};

// Takes a 1-indexed sound number, and tries replaces the current process with playing it
static int play_sound(int sound_number) {
    sound_number -= 1;
    assert(sound_number >= 0 && sound_number < SOUND_COUNT);

    // Use -n to provide a max length
    char* args[] = {"mpg123", "-n353", "-q", sounds[sound_number], NULL};
    return execvp("mpg123", args);
}

static pid_t spawn_alarm(int alarm_index, time_t sleep_time) {
    pid_t pid = fork();
    if (pid == 0) { // child process
        sleep(sleep_time);
        printf("RING RING!\n");
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);
        play_sound(alarms[alarm_index].sound_number);
        _exit(EXIT_FAILURE); // In case execvp fails. Does /not/ call atexit callbacks
    }

    return pid;
}

void add_alarm(time_t target_time, int sound_number) {
    cleanup_zombies();

    // Find an opening in the alarm list, or stop at the sentinel
    int alarm_index = 0;
    while (alarms[alarm_index].pid)
        alarm_index++;
    if (alarm_index >= MAX_ALARMS) {
        printf("Alarm list is full!\n");
        return;
    }

    if (sound_number < 1 || sound_number > SOUND_COUNT) {
        printf("Not a valid choice for sound!\n");
        return;
    }

    // Make sure the alarm is acutally in the future
    time_t now = time(NULL);
    if (target_time <= now) {
        printf("An alarm must be set in the future!\n");
        return;
    }

    // Print the number of seconds left
    time_t seconds_left = target_time - now;
    printf("Scheduling alarm in %ld seconds\n", seconds_left);

    // Store the alarm's target time, only used when printing the alarm list.
    // VERY important that all this data gets stored in the array before the fork happens
    alarms[alarm_index].time = target_time;
    alarms[alarm_index].sound_number = sound_number;
    alarms[alarm_index].has_been_listed = false;
    // Start the alarm frok and save its process id
    alarms[alarm_index].pid = spawn_alarm(alarm_index, seconds_left);
}

// Packs and sorts the alarm list by increasing ringing time
// Alarms with pid = 0 get placed at the very end
static int alarm_compare(const void* av, const void* bv) {
    const Alarm *a = av, *b = bv;
    // at and bt are the correct times for active alarms, and highest times for inactive
    time_t at = a->pid ? a->time : MAX(a->time,b->time)+1;
    time_t bt = b->pid ? b->time : MAX(a->time,b->time)+1;
    return at - bt;
}

// Shows a numbered list of current alarms, in increasing time of ringing
void list_alarms() {
    cleanup_zombies();
    qsort(alarms, MAX_ALARMS, sizeof(Alarm), alarm_compare);

    printf("Active alarms:\n");
    // Keep printing alarms until the first inactive alarm, or the sentinel alarm
    for (int i = 0; i < MAX_ALARMS; i++) {
        if (alarms[i].pid) {
            alarms[i].has_been_listed = true;
            printf("Alarm %2d at %s\n", (i+1), time_as_string(alarms[i].time));
        }
    }
}

static void kill_alarm(int i) {
    assert(alarms[i].pid);
    printf("Cancelling alarm that would ring at %s\n", time_as_string(alarms[i].time));
    kill(alarms[i].pid, SIGKILL);
}

// Lets you cancel alarms
// The alarm_number is 1-indexed, and the number is the same
// as the number printed at the last call to list_alarms(),
// even if the list has changed since then
void cancel_alarm(int alarm_number) {
    cleanup_zombies();
    alarm_number -= 1; // We 0-index internally
    if (alarm_number < 0 || alarm_number >= MAX_ALARMS
       || alarms[alarm_number].pid == 0) {
        printf("Not a valid alarm number!\n");
        return;
    }
    if (!alarms[alarm_number].has_been_listed) {
        printf("The alarm you are trying to cancel has never been listed. Generating new list.\n");
        list_alarms();
        printf("If you still want to cancel the alarm, find it on the list above.\n");
        return;
    }
    kill_alarm(alarm_number);
}

// Kills all active alarms that are still active
void cancel_all_alarms() {
    cleanup_zombies();
    for (int i = 0; i < MAX_ALARMS; i++) {
        if (!alarms[i].pid)
            continue;
        kill_alarm(i);
        waitpid(alarms[i].pid, NULL, 0); // Wait for signal to terminate
    }
}

// Checks all forks to see if they have finished.
// if so, clears them from the alarm list by setting pid = 0
void cleanup_zombies() {
    for (int i = 0; i < MAX_ALARMS; i++) {
        if (alarms[i].pid == 0)
            continue;
        // waitpid returns a positive number if the process has changed
        if (waitpid(alarms[i].pid, NULL, WNOHANG) > 0)
            alarms[i].pid = 0; //mark inactive
    }
}
