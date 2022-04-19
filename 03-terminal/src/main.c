#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <error.h>
#include <errno.h>
#include <stdbool.h>
#include "control.h"
#include "tasks.h"

#define MAX_LINE_LENGTH 256
#define MAX_TOKEN_COUNT 256

// The line read from user
static char line[MAX_LINE_LENGTH];
// The user input, divided into a list of tokens, null terminated
static char* argv[MAX_TOKEN_COUNT];

// Get one whole line of input from the user, exits if the line is too long or EOF.
// Consumes and removes the newline char.
// Line is stored in `line`, with the length returned.
static size_t readline() {
    // reads until it reads an EOF, a newline, or buffer is full
    char* result = fgets(line, sizeof(line), stdin);
    if (result == NULL) {
		putchar('\n'); // A last newline to finish the prompt line
        exit(EXIT_FAILURE);
	}

    // how long was the line we read?
    size_t len = strlen(line);

    // did we actually read a line? Only if line ends in \n
    if (line[len-1] != '\n')
        exit(EXIT_FAILURE);

    line[len-1]='\0'; //ignore the newline itself by moving null-terminator
    return len-1;
}

// Copies the read line into a list of tokens, argv, null terminated
// Returns the amount of tokens found. Not reentrant
static int getargs() {
	char* context = NULL;
	static char linecopy[MAX_LINE_LENGTH];
    strcpy(linecopy, line);

	char* token = strtok_r(linecopy, " \t", &context);
	int counter = 0;

	// Add each arg from input to array
	while (token != NULL)
	{
		assert(counter+1 < MAX_TOKEN_COUNT);
		argv[counter++] = token;
		token = strtok_r(NULL, " \t", &context);
	}
	// Finally add null termination
	argv[counter] = NULL;

	return counter;
}

int main(int flush_argc, char** flush_argv) {
	// The Shell's main REPL
	while (1) {
		// Before doing anyting, check for finished jobs
		cleanup_task_list();

		// Get current working directory
		char cwd[256];
		getcwd(cwd, sizeof(cwd));
		printf("%s: ", cwd);
		// Get a line of input from the user
		readline();
		// Tokenizes the line, stored in argv
		int argc = getargs();

		if (argc == 0) continue; // Ignore empty lines

		// Check for trailing " &"
		bool background = false;
		if (strcmp(argv[argc-1], "&") == 0) {
			argv[--argc] = NULL; // Move NULL terminator one back
			background = true;
		}

		char* inputpath = NULL;
		char* outputpath = NULL;
		bool pipeseen = false;

		for(int i = 1; i < argc;) { // Look for file pipes
			char* token = argv[i];
			if (strcmp(token, "<") == 0) {
				if (pipeseen) {
					error(0, 0, "Can't use both input redirection and input pipes at once");
					goto fail;
				}
				if (inputpath != NULL)  {
					error(0, 0, "Can't use multiple input redirections");
					goto fail;
				}

				inputpath = argv[i+1];
				if (inputpath == NULL) {
					error(0, 0, "error: no filepath given\n");
					goto fail;
				}
			}
			else if (strcmp(token, ">") == 0) {
				outputpath = argv[i+1];
				if (outputpath == NULL) {
					error(0, 0, "error: no filepath given\n");
					goto fail;
				}
			} else { // Anything other than "<" and ">"
				if (strcmp(token, "|") == 0) {
					pipeseen = true;
					if (outputpath != NULL) {
						error(0, 0, "Can't use both output redirection and output pipes at once");
						goto fail;
					}
				}

				i++;
				continue;
			}

			// We only end up here if there was a "<" or ">"
			for(int j = i; j+1 < argc; j++) // Remove the "<" or ">" and file path from list
				argv[j] = argv[j+2];
			argc -= 2; // we have two less tokens now
		}

		if (handle_builtin(argc, argv))
			continue;

		Task task;
		bool result = handle_command(&task, argc, argv, inputpath, outputpath);
		if (!result)
			continue;

		task.command = strdup(line);
		if (background) {
			add_to_task_list(task);
		} else {
			wait_for_task(&task);
		}

fail:;
	}
	
	return 0;
}
