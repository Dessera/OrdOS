#include "kernel/config/task.h"
#include "kernel/types.h"
#include "kernel/utils/bitmap.h"

struct pidpool
{
  struct atomic_bitmap pids;
  u8 buffer[TASK_PID_BITMAP_SIZE];
};

void
pidpool_init(struct pidpool* pool);

ssize_t
pidpool_alloc(struct pidpool* pool);

void
pidpool_free(struct pidpool* pool, size_t pid);