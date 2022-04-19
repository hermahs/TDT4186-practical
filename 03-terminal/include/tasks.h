#ifndef TASKS_H_
#define TASKS_H_

#include <unistd.h>

typedef struct {
   int stdout_pipe; // Reading end of output pipe
   pid_t pid;
   char* command; // The original command
} Task;

#define MAX_TASKS 16

void create_task(Task* task, char** argv, int inputfd, char* outputfile);
void wait_for_task(Task* task);
void add_to_task_list(Task task);
void cleanup_task_list();
void print_task_list();

#endif
