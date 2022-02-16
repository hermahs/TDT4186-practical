#include "timeutil.h"
#include <stdio.h>

// Gives the given time as a c string. The memory does not need to be freed.
// Subsequent calls will invalidate the string returned from previous calls
char* time_as_string(time_t time) {
    static char buffer[30];
    strftime(buffer, sizeof(buffer), "%F %T", localtime(&time));
    return buffer;
}

// Helper function to get the current time as a string using time_as_string's buffer
char* now_as_string() {
    time_t now = time(NULL);
    return time_as_string(now);
}

// Parses the provided time in the local time zone into a unix epoc stored in target
// Format: YYYY-MM-DD HH:MM:SS
// The numbers are allowed to use fewer digits, but never more
// Returns TIME_FAILED on failure
time_t parse_time(char *string) {
    struct tm time;
    time.tm_isdst = -1;
    char* not_read = strptime(string, "%F %T", &time);
    if (not_read == NULL || *not_read != '\0')
        return TIME_FAILED;
    return mktime(&time);
}
