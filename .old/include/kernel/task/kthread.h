#pragma once

#include "kernel/task/task.h"

void
init_kthread(void);

struct task*
kthread_run(char* name, u8 priority, task_entry_t function, void* arg);
