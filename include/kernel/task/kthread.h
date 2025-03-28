#pragma once

#include "kernel/task/task.h"

void
init_kthread(void);

struct task*
kthread_create(char* name, u8 priority, task_entry_t function, void* arg);

struct task*
kthread_get_idle(void);