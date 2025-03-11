#pragma once

#include "kernel/config/task.h"
#include "kernel/types.h"
#include "kernel/utils/list_head.h"
enum task_status
{
  TASK_STATUS_RUNNING,
  TASK_STATUS_READY,
  TASK_STATUS_BLOCKED,
  TASK_STATUS_WAITING,
  TASK_STATUS_SUSPENDED,
  TASK_STATUS_DEAD,
};

typedef void (*task_function_t)(void*);

struct task
{
  u32* kstack;
  enum task_status status;
  char name[TASK_NAME_SIZE];

  u8 priority;
  size_t ticks;
  size_t elapsed_ticks;

  struct list_head node;
  struct list_head global_node;

  u32* page_table;

  u32 tmagic;
};

void
init_task(void);