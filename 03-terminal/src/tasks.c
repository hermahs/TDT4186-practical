#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tasks.h"
#include "control.h"

#define MAX_SIZE 1024*1024
#define err(e) do {fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

Task tasklist[MAX_TASKS];

Task create_task(char* argc[]) {
    Task task;

    if (pipe(task.link) == -1) err("pipe");

    if((task.pid = fork()) == -1) err("fork");

    if (task.pid == 0) {
        dup2(task.link[1], STDOUT_FILENO);
        close(task.link[0]);
        execv(argc[0], argc);
        char* error_text = "Command not found (or some other error)";
        write(task.link[1], error_text, strlen(error_text) + 1);
        close(task.link[1]);
        exit(EXIT_FAILURE);
    }

    task.argc = argc[0];

    return task;
}

char* get_task_output(Task task) {
    int status;
    char output[MAX_SIZE];

    close(task.link[1]);
    int nbytes = read(task.link[0], output, sizeof(output));
    char* pretty_output = pretty_copy(output);
    memset(output, 0, strlen(output));
    return pretty_output;
}

char* get_task_status(Task task) {
    int status;
    pid_t w = waitpid(task.pid, &status, 0);

    if (w == -1) err("waitpid get_task_status");

    return add_status_ending(task.argc, status);
}

void add_to_task_list(Task task) {

    int task_index = 0;
    while(tasklist[task_index].pid)
        task_index++;
    
    if(task_index >= MAX_TASKS) {
        printf("Task list is full!\n");
        return;
    }

    tasklist[task_index] = task;
    return;

}

void cleanup_task_list() {
    pid_t w;
    int status;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (!tasklist[i].pid)
            continue;
        if ((w = waitpid(tasklist[i].pid, &status, WNOHANG)) > 0) {
            char* print_status = add_status_ending(tasklist[i].argc, status);
            printf("%s\n", print_status);
            free(print_status);
            tasklist[i].pid = 0;
        }
    }
}

void print_task_list() {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasklist[i].pid) {
            printf("%d: %s\n", tasklist[i].pid, tasklist[i].argc);
        }
    }
}