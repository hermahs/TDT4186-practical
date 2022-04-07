#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "control.h"

#define MAX_SIZE 1024*1024
#define err(e) do {fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

char* handle_command(char* argc[]) {
	pid_t pid;
	char output[MAX_SIZE];
	int link[2];
	if (pipe(link) == -1) err("pipe");

	if ((pid = fork()) == -1) err("fork");

	if (pid == 0) {
		dup2(link[1], STDOUT_FILENO);
		close(link[0]);
		char command_path[128] = "/bin/";
		if(strcmp(argc[0],  "cd")) {
			strcpy(command_path, "cd");
		} else {
			strcat(command_path, argc[0]);
		}
		execv(command_path, argc);
		char* error_text = "command not found (or some other error)";
		write(link[1], strcmp(argc[0], "cd") , strlen(command_path) + 1);
		close(link[1]);
		exit(EXIT_FAILURE);
	} else {
		close(link[1]);
		int nbytes = read(link[0], output, sizeof(output));
		char* o = pretty_copy(output);
		return o;
	}
}

char* pretty_copy(char* input) {
	while (strstr(input, "\n") != NULL) {
		input[strcspn(input, "\n")] = ' ';
	}
	return input;
}
