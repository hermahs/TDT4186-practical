#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include "util.h"
#include "not_found.h"

#define error(...) do { 		\
	fprintf(stderr, __VA_ARGS__); 	\
	exit(EXIT_FAILURE);		\
} while (false)

#define CRLF "\r\n"

/** Takes in a string where the first line looks like "GET /path HTTP/1.1"
  * and copies out the path part. Result needs to be free()d
  */
char* get_path(char* string) {
    char* context = NULL;
    char* firstline = strtok_r(string, "\n", &context);
    strtok_r(firstline, " ", &context);
    char *path = strtok_r(NULL, " ", &context);
    char* r = strdup(path);
    return r;
}

/** Takes in a path for requested file and serves it if found.
 *  If not found it will error (will send a 404 page later)
 */
char* get_file_from_path(char* path) {
	// TODO: add security so you can't go past base directory
	if (!check_if_path_exist(path))
		return not_found;

	char* buffer = 0;
	long length;
	FILE* html = fopen(path, "r");
	
	if (html) {
		fseek(html, 0, SEEK_END);
		length = ftell(html);
		fseek(html, 0, SEEK_SET);
		buffer = malloc(length);
		if (buffer) {
			fread(buffer, 1, length, html);
		}
		fclose(html);
	}

	return buffer;	
}

bool check_if_path_exist(char* path) {
	DIR* dir = opendir(path);

	if(ENOENT == errno) return false;

	closedir(dir);
	
	return true;
}

int create_send_data_to_client() {
	char* path = get_path(recv_buffer);
	char full_path[256];

	free(path);
	char* file_content = get_file_from_path(full_path);

	if (file_content != NULL) {
		int send = snprintf(send_buffer, sizeof(send_buffer),
							"HTTP/1.0 200 OK"CRLF
							"Content-Length: %ld"CRLF
							"Content-Type: text/html; charset=utf-8"CRLF
							CRLF
							"%s",
							strlen(file_content), file_content);
	} else {
		// 404
	}

	free(file_content);

	if (send >= sizeof(send_buffer))
		printf("warning: output was truncated!\n");

	return send;
}	

void* send_data_to_client() {

}
