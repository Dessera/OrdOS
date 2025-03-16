#pragma once

#include "kernel/utils/list_head.h"
#include "lib/types.h"

struct spin_lock
{
  u8 flag;
};

void
spin_lock_init(struct spin_lock* lock);
void
spin_lock(struct spin_lock* lock);
void
spin_unlock(struct spin_lock* lock);

struct mutex_lock
{
  u8 flag;
  struct spin_lock guard;
  struct list_head wait_queue;
};

void
mutex_lock_init(struct mutex_lock* lck);
void
mutex_lock(struct mutex_lock* lck);
void
mutex_unlock(struct mutex_lock* lck);

bool
intr_lock(void);
void
intr_unlock(bool flag);