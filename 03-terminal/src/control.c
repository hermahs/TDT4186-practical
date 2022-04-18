#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "control.h"
#include "tasks.h"

#define MAX_SIZE 1024*1024
#define err(e) do {fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

Task tasklist[16];

char* add_status_ending(char* command, int status) {
	char* ending_string = malloc(sizeof(char) * (19 + strlen(command) + 1)); // Exit status [
	sprintf(ending_string, "Exit status [%s] = %d", command, status);
	return ending_string;
}

char* pretty_copy(char* input) {
	char* r = malloc(sizeof(char) * strlen(input) + 1);
	strcpy(r, input);
	while (strstr(r, "\n") != NULL) {
		r[strcspn(r, "\n")] = ' ';
	}
	return r;
}

char* handle_command(char* argc[]) {

	// check if background tasks are done

	if (strcmp(argc[0], "cd") == 0) {
		if (chdir(argc[1]) != 0)
			return "could not find directory";
		return "";
	} else if (strcmp(argc[0], "jobs") == 0) {
		// add task to tasklist
		return "job stuff coming\n";
	}

	// handle background task stuff here
	if (argc[0][strlen(argc[0] - 1)] == '&') {

	} else {
		Task task = create_task(argc);
		return get_task_output(task);
	}



}
