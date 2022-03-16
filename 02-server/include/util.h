#ifndef __UTIL_H__
#define __UTIL_H__

typedef struct {
	char* origin_path;
	int port;
	int num_threads;
	int buffer_slots;	
} settings_s;

extern __thread char recv_buffer[(1024*1024)], send_buffer[(1024*1024)];
extern settings_s settings;

char* get_path(char* string);

char* get_file_from_path(char* path);

bool check_if_path_exist(char* path);

int create_send_data_to_client();

#endif //__UTIL_H_
