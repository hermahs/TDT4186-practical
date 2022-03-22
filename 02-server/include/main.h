#ifndef MAIN_H_
#define MAIN_H_

typedef struct {
	char* origin_path;
	int port;
	int num_threads;
	int buffer_slots;
} settings_s;

extern settings_s settings;

#endif // MAIN_H_
