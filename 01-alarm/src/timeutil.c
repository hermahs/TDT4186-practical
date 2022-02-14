#include "timeutil.h"
#include <time.h>

// Gives the current time as a c string. The memory does not need to be freed.
// Subsequent calls will invalidate the string returned from previous calls
char* now_as_string() {
    time_t now;
    time(&now);
    static char buffer[30];
    strftime(buffer, sizeof(buffer), "%F %T", localtime(&now));
    return buffer;
}
