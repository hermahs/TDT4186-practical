#ifndef TASKS_H_
#define TASKS_H_

typedef struct {
   int link[2];
   pid_t pid;
   char* argc;
} Task;

#define MAX_TASKS 16

Task create_task(char* argc[]);
char* get_task_output(Task task);
char* get_task_status(Task task);
void add_to_task_list(Task task);
void cleanup_task_list();
void print_task_list();

#endif