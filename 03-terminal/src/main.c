#include <stdio.h>
#include <string.h>
#include "main.h"
#include "stdlib.h"

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
	char command[50];
	while (1)
	{
		printf("Something something path: ");
		readline();
		printf("%s \n", line);
	}
	
	return 0;
}
