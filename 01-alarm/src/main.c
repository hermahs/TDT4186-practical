#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "timeutil.h"
#include "alarm.h"

// A helper function for reading one line from stdin.
// It will also consume the trailing newline, but not include it in the output string.
// Returns the length of the read string, excluding newline and 0-terminator.
// If EOF is reached, or a super long line is entered, the function never returns, and exit is called
static char line[256];
static size_t readline() {

    // reads until it reads an EOF, a newline, or buffer is full
    char* result = fgets(line, sizeof(line), stdin);
    if(result == NULL)
        exit(EXIT_FAILURE);

    // how long was the line we read?
    size_t len = strlen(line);

    // did we actually read a line? Only if line ends in \n
    if(line[len-1] != '\n')
        exit(EXIT_FAILURE);

    line[len-1]='\0'; //ignore the newline itself by moving null-terminator
    return len-1;
}

/// Handles one option in the main menu, returns false if the option was unrecognized
bool handle_menu_option(char c) {
    switch (c)
    {
        case 's':
            printf("Schedule alarm at which date and time? ");
            readline();
            time_t target_time = parse_time(line);
            if (target_time == TIME_FAILED) {
                printf("Time is incorrectly formated!\n");
                break;
            }
            printf("Select the sound for your alarm (1-%d, default 1): ", SOUND_COUNT);
            int input_len = readline();
            int sound_number;
            if (input_len == 0) sound_number = 1;
            else if(input_len == 1) sound_number = line[0]-'0';
            else {
                printf("Not a valid number entry!\n");
                break;
            }

            add_alarm(target_time, sound_number);
            break;

        case 'l':
            list_alarms();
            break;

        case 'c':
            printf("Cancel which alarm? ");
            readline();
            int alarm_number; char dummy_char;
            if(sscanf(line, "%d%c", &alarm_number, &dummy_char) == 1)
                cancel_alarm(alarm_number);
            else
                printf("Not a valid number entry!\n");
            break;

        case 'x':
            printf("Goodbye!\n");
            cancel_all_alarms();
            exit(EXIT_SUCCESS);

        default:
            return false;
    }
    return true;
}

int main() {
    printf("Welcome to the alarm clock! It is currently %s\n", now_as_string());
    printf("Please enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)\n");

    while(1) {
        printf("> ");
        int read = readline();
        if (read == 0) continue; //Just pressing enter is ok
        if (read == 1 && handle_menu_option(line[0]))
            continue;
        printf("Invalid option\n");
    }
}
