#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

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

char* get_file_from_path(char* path) {
	// TODO: add security so you can't go past base directory

	// See if file exists in folder


}
