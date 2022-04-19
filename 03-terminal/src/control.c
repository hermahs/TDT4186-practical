#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <error.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include "control.h"
#include "tasks.h"

// Tries to handle the given command as a builtin.
// Returns true if it was a builtin
bool handle_builtin(int argc, char** argv) {
	assert(argc >= 1); // Not given an empty list
	char* command = argv[0];

	if (strcmp(command, "cd") == 0) {
		if (argc != 2)
			error(0, 0, "Expected exactly one argument");
		else if (chdir(argv[1]) != 0)
			error(0, 0, "Could not find directory");
		return true;
	} else if (strcmp(command, "jobs") == 0) {
		if (argc != 1)
			error(0, 0, "Unexpected arguments");
		print_task_list();
		return true;
	}

	return false;
}

// Handles the given command, or commands, if pipes are present
// Takes optional input and output paths, if NULL uses stdout and stdin
// These apply only to the very first and last programs in the pipe chain
// Returns false if some error prevented the commands from running
bool handle_command(Task* task, int argc, char** argv, char* inputpath, char* outputpath) {
	// Set up the input to the start of the chain
	int inputfd = STDIN_FILENO; // Deafult is to use stdin
	if (inputpath) {
		inputfd = open(inputpath, O_RDONLY);
		if (inputfd == -1) {
			error(0, errno, "input file redirection");
			return false;
		}
	}

	char** current_argv = argv;
	int current_argc = 0;
	while(current_argv[current_argc] != NULL) {
		if(strcmp(current_argv[current_argc], "|") == 0) {
			current_argv[current_argc] = NULL; //Insert a new null terminator
			create_task(task, current_argv, inputfd, "PIPE");
			inputfd = task->stdout_pipe;
			current_argv += current_argc+1;
			current_argc = 0;
		} else
			current_argc++;
	}

	create_task(task, current_argv, inputfd, outputpath);

	return true;
}
