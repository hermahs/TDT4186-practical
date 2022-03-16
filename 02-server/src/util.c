#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include "util.h"

char* get_path(char* string) {
    char* context = NULL;
    char* string_copy = malloc(sizeof(char) * strlen(string) + 1);
    // invalid write
    strcpy(string_copy, string);
    char* get_segment = strtok_r(string_copy, "\n", &context);
    char* path = strtok_r(get_segment, " ", &context);
    path = strtok_r(NULL, " ", &context);
    char* r = malloc(sizeof(char) * strlen(path) + 1);
    strcpy(r, path);
    free(string_copy);
    return r;
}

char* get_file_from_path(char* path) {
	// TODO: add security so you can't go past base directory

	// See if file exists in folder


}

bool check_if_path_exist(char* path) {
	DIR* dir = opendir(path);

	if(ENOENT == errno) return false;

	closedir(dir);
	
	return true;
}
