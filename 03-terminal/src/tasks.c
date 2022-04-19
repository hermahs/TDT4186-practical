#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <error.h>
#include <errno.h>
#include <fcntl.h>
#include "tasks.h"

Task tasklist[MAX_TASKS];

// Spawns a new process for the given program.
// If outputfile != NULL, it is used as the stdout of the child.
// if outputfile == "PIPE", a pipe is created, and the reading end is stored in the task.
void create_task(Task* task, char** argv, int inputfd, char* outputfile) {

    task->stdout_pipe = 0;

    bool output_pipe = outputfile != NULL && (strcmp(outputfile, "PIPE") == 0);
    int output_pipes[2];
    if (output_pipe) {
        if (pipe(output_pipes) != 0)
            error(1, errno, "pipe");
        task->stdout_pipe = output_pipes[0]; // The reading end of the pipe
    }

    if((task->pid = fork()) == -1) error(1, errno, "fork");

    if (task->pid == 0) {
        // We are the child
        // Replace STDIN and/or STDOUT with files or pipes if provided
        dup2(inputfd, STDIN_FILENO);

        if (output_pipe) {
            dup2(output_pipes[1], STDOUT_FILENO); // stdout is now writing end of pipe
            //close(output_pipes[0]); // close reading end
        } else if (outputfile) { // Normal file redirection
            int outfile = open(outputfile, O_WRONLY|O_CREAT, 0644);
            if (outfile == -1)
                error(1, errno, "stdout redirection");
            dup2(outfile, STDOUT_FILENO);
        }

        execvp(argv[0], argv);

        // execv is not supposed to return, so something must have failed
        // Print to stderr, since stdout is now piped
        fprintf(stderr, "Command not found (or some other error)\n");
        exit(EXIT_FAILURE);
    }

    // The parent should close the ends of the pipes it doesn't use
    if (output_pipe)
        close(output_pipes[1]); // Close the writing end of the childs stdout
}

void close_task(Task* task, int status) {
    printf("Exit status [%s] = %d\n", task->command, WEXITSTATUS(status));
    task->pid = 0;
    free(task->command);

}

void wait_for_task(Task* task) {
    int status;
    pid_t w = waitpid(task->pid, &status, 0);
    if (w == -1) error(1, errno, "waitpid get_task_status");

    close_task(task, status);
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
}

void cleanup_task_list() {
    pid_t w;
    int status;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (!tasklist[i].pid)
            continue;
        if ((w = waitpid(tasklist[i].pid, &status, WNOHANG)) > 0)
            close_task(&tasklist[i], status);
    }
}

void print_task_list() {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasklist[i].pid) {
            printf("%d: %s\n", tasklist[i].pid, tasklist[i].command);
        }
    }
}
