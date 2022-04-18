#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "control.h"
#include "io.h"
#include "tasks.h"

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
	char* context = NULL;
	char* linecpy;
    linecpy = (char*)malloc(sizeof(line));
    strcpy(linecpy, line);
	char delim[] = " \t";
	char* token = strtok_r(linecpy, delim, &context);
	int counter = 0;

	// Add each arg from input to array
	while (token != NULL)
	{
		ch_arr[counter] = token;
		counter ++;
		token = strtok_r(NULL, delim, &context);
	}
	ch_arr[counter] = NULL;

	return counter;
}

int main(int argc, char* argv) {
	getcwd(cwd, sizeof(cwd));

	while (1)
	{
		to_file = 0;
		// Get current working directory
		cleanup_task_list();
		getcwd(cwd, sizeof(cwd));
		printf("%s: ", cwd);
		// Leser inn bruker input, altså command med args
		readline();
		getargs();

		// this can be taken out into its own function (i think)
		int i = 0;
		while (ch_arr[i] != NULL) {
			if (strcmp(ch_arr[i], "<") == 0) {
				// ch_arr[i] = ch_arr[i + 1];
				// if (ch_arr[i + 1] == NULL) {
				// 	printf("Error, no filepath given");
				// 	break;
				// }
				// strncpy(fileinput, file_in(ch_arr[i + 1]), 256);
				// ch_arr[i] = fileinput;
				int j = i; // Skal være int j = i + 1 hvis det skal være med resten av koden over
				while (ch_arr[j] != NULL) {
					ch_arr[j] = ch_arr[j + 1];
					j++;
				}
			}
			if (strcmp(ch_arr[i], ">") == 0) {
				if (ch_arr[i + 1] == NULL) {
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

		// can read from file in background but not create file in background
		Data output = handle_command(ch_arr);
		if (to_file == 1) {
			file_out(to_file_path, output.output);
			printf("%s\n", output.status);
		} else {
			if(strcmp(output.output, "\0") != 0) {
				printf("%s\n%s\n", output.output, output.status);
			}
		}
		free(output.output);
		free(output.status);
	}
	
	return 0;
}
