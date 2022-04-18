#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "io.h"

static char text[256];

char* file_in(char* filepath) {
    FILE *file = fopen(filepath, "r");

    if (file == NULL) {
        printf("Could not read file \n");
    }
    fscanf(file, "%s", &text);
    //printf("What is hello in spanish? \n%s \n", text);
    fclose(file);
    return text;
}

int file_out(char* filepath, char* data) {
    FILE *file = fopen(filepath, "w");
    
    fprintf(file, "%s", data);
    fclose(file);

    return 0;
}