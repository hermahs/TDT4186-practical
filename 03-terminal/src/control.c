#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "control.h"

#define MAX_SIZE 1024*1024
#define err(e) do {fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

char* handle_command(char* command, char* args) {
	pid_t pid;
	char output[MAX_SIZE];
	int link[2];
	if (pipe(link) == -1) err("pipe");

	if ((pid = fork()) == -1) err("fork");

	if (pid == 0) {
		dup2(link[1], STDOUT_FILENO);
		close(link[0]);
		close(link[1]);
		execl("/bin/ls", "ls", "-1", (char*) NULL);
		err("execl");
	} else {
		close(link[1]);
		int nbytes = read(link[0], output, sizeof(output));
		char* o = pretty_copy(output);
		return o;
	}

	
		
}

const char* get_file_from_command(char* command) {
/*
	switch(command) {
		case "ls":
			const char* c =  "/bin/ls";	
			return c;
	}
*/
}

char* pretty_copy(char* input) {
	while (strstr(input, "\n") != NULL) {
		input[strcspn(input, "\n")] = ' ';
	}
	return input;
}
