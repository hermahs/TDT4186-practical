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
    pid_t w;

    w = waitpid(task.pid, &status, 0);

    if (w == -1) err("waitpid");

    close(task.link[1]);
    int nbytes = read(task.link[0], output, sizeof(output));
    char* pretty_output = pretty_copy(output);
    char* ending = add_status_ending(task.argc, status);
    char* ret_output = malloc(sizeof(char) * (strlen(pretty_output) + strlen(ending) + 2));
    sprintf(ret_output, "%s\n%s", pretty_output, ending);
    free(pretty_output);
    free(ending);
    memset(output, 0, strlen(output));
    return ret_output;
}

int get_task_status(Task task) {
    int status;
    pid_t w;

    if ((w = waitpid(task.pid, &status, 0)) == -1) err("waitpid");

    return status;
}