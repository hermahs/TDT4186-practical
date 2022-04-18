#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "control.h"
#include "io.h"

static int to_file = 0;
static char to_file_path[256];
static char cwd[256];
static char line[256];
static char* ch_arr[256];
static char fileinput[256];
static size_t readline() {

    // reads until it reads an EOF, a newline, or buffer is full
    char* result = fgets(line, sizeof(line), stdin);
    if (result == NULL)
        exit(EXIT_FAILURE);

    // how long was the line we read?
    size_t len = strlen(line);

    // did we actually read a line? Only if line ends in \n
    if (line[len-1] != '\n')
        exit(EXIT_FAILURE);

    line[len-1]='\0'; //ignore the newline itself by moving null-terminator
    return len-1;
}

static int getargs() {

	char* linecpy;
    linecpy = (char*)malloc(sizeof(line));
    strcpy(linecpy, line);
	char* token = strtok(linecpy, " ");
	int counter = 0;

	// Add each arg from input to array
	while (token != NULL)
	{
		ch_arr[counter] = token;
		counter ++;
		token = strtok(NULL, " ");
	}
	ch_arr[counter] = NULL;

	return counter;
}

int main(int argc, char* argv) {
	// Get current working directory
	// char* t[] = {"cd", "..", NULL};
	// char* yeet = handle_command(t);
	// printf("%s\n", yeet);
	getcwd(cwd, sizeof(cwd));

	while (1)
	{
		// Get current working directory
		getcwd(cwd, sizeof(cwd));
		printf("%s: ", cwd);
		// Leser inn bruker input, altså command med args
		readline();
		getargs();
		int i = 0;
		while (ch_arr[i] != NULL) {
			if (strcmp(ch_arr[i], "<") == 0){
				if (ch_arr[i + 1] == NULL){
					printf("Error, no filepath given");
					break;
				}
				strncpy(fileinput, file_in(ch_arr[i + 1]), 256);
				ch_arr[i] = fileinput;
				int j = i + 1;
				while (ch_arr[j] != NULL) {
					ch_arr[j] = ch_arr[j + 1];
					j++;
				}
			}
			if (strcmp(ch_arr[i], ">") == 0){
				if (ch_arr[i + 1] == NULL){
					printf("Error, no filepath given");
					break;
				}
				to_file = 1; 
				strncpy(to_file_path, ch_arr[i + 1], 256);
				ch_arr[i] = ch_arr[i + 2];
				ch_arr[i + 1] = NULL;
			}
			i++;
		}
		char* output = handle_command(ch_arr);
		printf("%s\n", output);
		if (to_file == 1){
			file_out(to_file_path, output);
		}
		free(output);
	}
	
	return 0;
}
