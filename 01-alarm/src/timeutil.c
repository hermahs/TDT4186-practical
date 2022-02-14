#include "timeutil.h"
#include <stdio.h>

// Gives the current time as a c string. The memory does not need to be freed.
// Subsequent calls will invalidate the string returned from previous calls
char* now_as_string() {
    time_t now;
    time(&now);
    static char buffer[30];
    strftime(buffer, sizeof(buffer), "%F %T", localtime(&now));
    return buffer;
}

// Parses the provided time in the local time zone into a unix epoc stored in target
// Format: YYYY-MM-DD HH:MM:SS
// The numbers are allowed to use fewer digits, but never more
// Returns TIME_FAILED on failure
time_t parse_time(char *string) {
    struct tm time;
    char dummy_char;
    int scanned = sscanf(string, "%4d-%2d-%2d %2d:%2d:%2d%c",
                         &time.tm_year, &time.tm_mon, &time.tm_mday,
                         &time.tm_hour, &time.tm_min, &time.tm_sec,
                         &dummy_char);
    if (scanned != 6) // We didn't read all the fields, or read too many!
        return TIME_FAILED;
    return mktime(&time);
}
