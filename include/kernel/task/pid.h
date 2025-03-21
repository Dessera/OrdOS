#pragma once

#include "kernel/config/task.h"
#include "kernel/task/sync.h"
#include "kernel/utils/bitmap.h"
#include "lib/types.h"

struct pidpool
{
  struct bitmap pids;
  struct spin_lock lock;
  u8 buffer[TASK_PID_BITMAP_SIZE];
};

void
pidpool_init(struct pidpool* pool);

ssize_t
pidpool_alloc(struct pidpool* pool);

void
pidpool_free(struct pidpool* pool, size_t pid);