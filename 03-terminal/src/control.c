#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "control.h"

#define MAX_SIZE 1024*1024
#define err(e) do {fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

char* handle_command(char* command, char* args) {
	pid_t pid;
	char output[MAX_SIZE];

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
		printf("Output: %s\n", output);
		return output;
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
