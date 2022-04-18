#ifndef CONTROL_H_
#define CONTROL_H_

typedef struct {
    char* output;
    char* status;
} Data;

Data handle_command(char* argc[]);
char* pretty_copy(char* input);
char* print_output_of_func(char* command, int status);
char* add_status_ending(char* command, int status);

#endif
