#ifndef MAIN_H_
#define MAIN_H_

#include <threads.h>

typedef struct {
	char* origin_path;
	int port;
	int num_threads;
	int buffer_slots;
} settings_s;

extern settings_s settings;

#define CRLF "\r\n"
#define BUFFER_SIZE (1024*1024)
extern thread_local char recv_buffer[BUFFER_SIZE], send_buffer[BUFFER_SIZE];
extern thread_local int thread_number;

#endif // MAIN_H_
