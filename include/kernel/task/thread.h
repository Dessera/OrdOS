#pragma once

#include "kernel/task/task.h"

void
init_thread(void);

struct task_t*
thread_run(char* name, u8 priority, task_function_t function, void* arg);

struct task_t*
thread_current(void);

void
thread_schedule(void);

void
thread_yield(void);

void
thread_park(void);

void
thread_unpark(struct task_t* task);