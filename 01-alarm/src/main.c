#include <stdio.h>
#include <time.h>
#include "timeutil.h"

// 1. meny rutine

// 2. Datalagring

// 3. Alarm planlegging

// 4. Avbryting av alarmer

// 5. Fang zombiene!

// 6. En ordentlig ringelyd

// 7. Test case dokumentasjon

void menu() {
    char input[2];

    printf("Welcome to the alarm clock! It is currently %s\n", now_as_string());
    printf("Please enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)\n");

prompt:
    printf("> ");
    scanf("%1s", input);

    switch (input[0])
    {
    case 's':
        printf("hei magnus\n");
        break;
    
    case 'l':
        break;

    case 'c':
        break;

    case 'x':
        printf("Goodbye!\n");
        return;

    default:
        printf("Please enter a valid input.\n");
        break;
    }
    goto prompt;
}


int main() {
    menu();
    return 0;
}