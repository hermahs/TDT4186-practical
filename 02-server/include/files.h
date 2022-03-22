#ifndef FILES_H_
#define FILES_H_

#include <stdbool.h>

char* get_path(char* string);

int get_file_from_path(char** dest, const char* dir, const char* path);

bool check_if_path_readable(const char* path);

bool has_extension(const char* path, const char* extension);

#endif // FILES_H_
