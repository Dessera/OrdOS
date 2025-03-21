#pragma once

#include "kernel/config/task.h"
#include "kernel/utils/list_head.h"
#include "lib/types.h"

typedef void (*task_entry_t)(void*);

/**
 * @brief Task status enumeration
 */
enum task_status
{
  TASK_STATUS_RUNNING,
  TASK_STATUS_READY,
  TASK_STATUS_BLOCKED,
  TASK_STATUS_DEAD,
};

struct task
{
  void* stack;
  size_t pid;
  enum task_status status;
  char name[TASK_NAME_SIZE];

  u8 priority;
  size_t ticks;
  size_t elapsed_ticks;

  struct list_head node;
  struct list_head global_node;

  u32* page_table;
};

void
init_task(void);

void
task_init(struct task* task, const char* name, u8 priority);

void
task_init_stack(struct task* task, task_entry_t function, void* arg);

void
task_push_global(struct task* task);

void
task_push(struct task* task);

void
task_set_current(struct task* task);

struct task*
task_get_current(void);

void
task_yield(void);

void
task_park(void);

void
task_unpark(struct task* task);