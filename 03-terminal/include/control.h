#ifndef CONTROL_H_
#define CONTROL_H_

#include <stdbool.h>
#include "tasks.h"

bool handle_builtin(int argc, char** argv);

bool handle_command(Task* task, int argc, char** argv, char* inputpath, char* outputpath);

#endif
