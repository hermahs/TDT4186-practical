#ifndef _TASKS_H
#define _TASKS_H

typedef struct Tasks {
   int link[2];
   pid_t pid;
   char* argc;
} Task;

Task create_task(char* argc[]);
char* get_task_output(Task task);
int get_task_status(Task task);

#endif