#pragma once

#include "ordos/config.h"
#include "ordos/lib/sync.h"
#include "ordos/lib/types.h"
#include "ordos/lib/util/bitmap.h"

struct pidpool
{
  struct bitmap pids;
  struct spin_lock lock;
  u8 buffer[ORDOS_TASK_PIDPOOL_SIZE];
};

void
pidpool_init(struct pidpool* pool);

ssize_t
pidpool_alloc(struct pidpool* pool);

void
pidpool_free(struct pidpool* pool, size_t pid);
