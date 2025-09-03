#pragma once

#include "ordos/config.h"
#include "ordos/lib/util/list_head.h"

typedef void (*task_sched_t)(struct list_head*);

struct sched
{
  char name[ORDOS_TASK_SCHED_NAME_LENGTH];
  task_sched_t entry;  /**< Normal scheduler path. */
  task_sched_t fentry; /**< Force scheduler path */
};

int
sched_register(const char* name, task_sched_t entry, task_sched_t fentry);

struct sched*
sched_find(const char* name);
