#ifndef TIME_H_
#define TIME_H_
#include <time.h>

#define TIME_FAILED ((time_t)-1)

char* now_as_string();
time_t parse_time(char *string);

#endif // TIME_H_
