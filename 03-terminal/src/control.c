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

Data handle_command(char* argc[]) {

	Data ret_data;
	ret_data.output = malloc(sizeof(char));
	sprintf(ret_data.output, "%s", "\0");
	ret_data.status = malloc(sizeof(char));
	sprintf(ret_data.status, "%s", "\0");

	// gonna give us seg.faults in free in main
	if (strcmp(argc[0], "cd") == 0) {
		if (chdir(argc[1]) != 0) {
			ret_data.output = malloc(sizeof(char) * 25);
			sprintf(ret_data.output, "%s", "Could not find directory");
		}
		return ret_data;
	} else if (strcmp(argc[0], "jobs") == 0) {
		print_task_list();
		return ret_data;
	}

	Task task;

	// handle background task stuff here
	int len = 0;
	while (argc[len] != NULL) {
		len++;
	}
	
	if (strcmp(argc[len - 1], "&") == 0) {
		argc[len - 1] = NULL;
		task = create_task(argc);
		add_to_task_list(task);
	} else {
		task = create_task(argc);
		ret_data.output = get_task_output(task);
		ret_data.status = get_task_status(task);
	}

	return ret_data;
}
