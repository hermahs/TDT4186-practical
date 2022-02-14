#ifndef TIME_H_
#define TIME_H_
#define _XOPEN_SOURCE
#include <time.h>

#define TIME_FAILED ((time_t)-1)

char* time_as_string(time_t time);
char* now_as_string();
time_t parse_time(char *string);

#endif // TIME_H_
