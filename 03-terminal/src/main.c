#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "main.h"
#include "control.h"

static char cwd[256];
static char line[256];
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

int main(int argc, char* argv) {

	char* yeet = handle_command("yeet", "yeet"); 
	
	printf("%s:%ld", yeet, sizeof(yeet));

	while (1)
	{
		// Get current working directory
		getcwd(cwd, sizeof(cwd));
		printf("%s: ", cwd);
		// Leser inn bruker input, altså command med args
		readline(); 
		printf("%s \n", line);
	}
	
	return 0;
}
