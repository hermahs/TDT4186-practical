#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include "util.h"

#define error(...) do { 			\
	frpintf(stderr, __VA_ARGS__); 	\
	exit(EXIT_FAILURE);				\
} while (false)

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

char* get_file_from_path(char* patih) {
	// TODO: add security so you can't go past base directory
	if (!check_if_path_exist(path))
		error("file does not exist\n"); // send 404 here later

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
