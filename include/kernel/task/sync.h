#pragma once

#include "kernel/interrupt/interrupt.h"
#include "kernel/utils/list_head.h"
#include "lib/types.h"

struct spin_lock
{
  volatile i32 flag;
};

void
spin_lock_init(struct spin_lock* lock);
void
spin_lock(struct spin_lock* lock);
void
spin_unlock(struct spin_lock* lock);

static FORCE_INLINE bool
intr_lock(void)
{
  return intr_set_status(false);
}

static FORCE_INLINE void
intr_unlock(bool flag)
{
  intr_set_status(flag);
}

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

struct semaphore
{
  size_t value;
  struct spin_lock guard;
  struct list_head wait_queue;
};

void
semaphore_init(struct semaphore* sem, size_t value);

void
semaphore_down(struct semaphore* sem);

void
semaphore_up(struct semaphore* sem);