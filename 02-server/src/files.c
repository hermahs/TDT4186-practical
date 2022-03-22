#include "files.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <unistd.h>

#define CRLF "\r\n"

/** Takes in a string where the first line looks like "GET /path HTTP/1.1,
  * modifies it and returns a pointer to the path part
  */
char* get_path(char* string) {
    char* context = NULL;
    strtok_r(string, " ", &context);
    char *path = strtok_r(NULL, " \n\r", &context);
    return path;
}

/** Takes in a dir and a path and merges them to form a complete path.
 * If the path part contains any "/.", the '.' is skipped until there are no "/."
 * Same goes for "//", getting turned into a single "/".
 * Returns the resulting string, must be freed.
 *
 * Note: This is not the proper way of securing a web server's access.
 * You should run the server as a user that only has access to the www-path folder.
 */
char* join_path(const char* dir, const char* path) {
	size_t dirlen = strlen(dir);
	size_t pathlen = strlen(path);
	char* result = malloc(dirlen+pathlen+2);
	strcpy(result, dir);
	char* dest = result+dirlen;
	*(dest++) = '/'; // Assure we have at least one slash, double slash is ok
	for(int i = 0; i < pathlen; i++) {
		if(dest[-1] == '/' && (path[i] == '.' || path[i] == '/'))
			continue;
		*(dest++) = path[i];
	}
	*dest = '\0'; // Null terminator
	return result;
}

/** Takes in a dir and path for a requested file and reads it into memory,
 *  and saves a pointer to it into dest. The data must be freed.
 *  Returns size of file, or -1 in case of errors.
 */
int get_file_from_path(char** dest, const char* dir, const char* path) {
	char* fullpath = join_path(dir, path);

	if (!check_if_path_readable(fullpath)) {
		error(0, errno, "not a readable file: %s", fullpath);
		free(fullpath);
		return -1;
	}

	FILE* file = fopen(fullpath, "r");
	
	if (!file) {
		error(0, errno, "Could not open file: %s", fullpath);
		free(fullpath);
		return -1;
	}

	fseek(file, 0, SEEK_END);
	int length = ftell(file);
	fseek(file, 0, SEEK_SET);
	*dest = malloc(length);
	if (!*dest)
		error(-1, errno, "Out of memory");
	fread(*dest, 1, length, file);
	fclose(file);

	free(fullpath);
	return length;
}

bool check_if_path_readable(const char* path) {
	return access( path, R_OK ) == 0;
}

bool has_extension(const char* path, const char* extension) {
	int pathlen = strlen(path);
	int extlen = strlen(extension);
	if(extlen > pathlen) return false;
	for(int i = 1; i <= extlen; i++)
		if (path[pathlen-i] != extension[extlen-i]) return false;
	return true;
}
