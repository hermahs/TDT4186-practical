#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "io.h"

//static char text[256];

char* file_in(char* filepath) {
    FILE *file = fopen(filepath, "rb");

    if (file == NULL) {
        printf("Could not read file: %s \n", filepath);
    }
    fseek(file, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(file, 0, SEEK_SET);

    char *text = malloc(fsize + 1);
    fread(text, fsize, 1, file);
    fclose(file);
    text[fsize] = 0;
    
    return text;
}

int file_out(char* filepath, char* data) {
    FILE *file = fopen(filepath, "w");
    
    fprintf(file, "%s", data);
    fclose(file);

    return 0;
}