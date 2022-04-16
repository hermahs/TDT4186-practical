#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "control.h"

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

char* handle_command(char* argc[]) {
	if (strcmp(argc[0], "cd") == 0) {
		if (chdir(argc[1]) != 0)
			return "could not find directory";
		return "";
	}

	pid_t pid, w;
	char output[MAX_SIZE];
	int link[2];
	int status;
	if (pipe(link) == -1) err("pipe");

	if ((pid = fork()) == -1) err("fork");

	if (pid == 0) {
		dup2(link[1], STDOUT_FILENO);
		close(link[0]);
		execv(argc[0], argc);
		char* error_text = "command not found (or some other error)";
		write(link[1], error_text , strlen(error_text) + 1);
		close(link[1]);
		exit(EXIT_FAILURE);
	} else {
		
		w = waitpid(pid, &status, WUNTRACED | WCONTINUED);

		if (w == -1) {
			err("waitpid");
		}

		close(link[1]);
		int nbytes = read(link[0], output, sizeof(output));
		char* pretty_output = pretty_copy(output);
		char* ending = add_status_ending(argc[0], status);
		printf("%ld, %ld\n", strlen(pretty_output), strlen(ending));
		char* ret_output = malloc(sizeof(char) * (strlen(pretty_output) + strlen(ending) + 2));
		sprintf(ret_output, "%s\n%s", pretty_output, ending);
		free(pretty_output);
		free(ending);
		memset(output, 0, strlen(output));
		return ret_output;
	}
}
