#pragma once

#include "kernel/types.h"
#include "kernel/utils/list_head.h"

struct spin_lock_t
{
  u8 flag;
};

void
spin_lock_init(struct spin_lock_t* lock);
void
spin_lock(struct spin_lock_t* lock);
void
spin_unlock(struct spin_lock_t* lock);

struct lock_t
{
  u8 flag;
  struct spin_lock_t guard;
  struct list_head wait_queue;
};

void
lock_init(struct lock_t* lck);
void
lock(struct lock_t* lck);
void
unlock(struct lock_t* lck);

bool
intr_lock(void);
void
intr_unlock(bool flag);

static FORCE_INLINE u8
atomic_cmpexchange(u8* ptr, u8 old, u8 new)
{
  u8 ret;

  __asm__ __volatile__("lock;"
                       "cmpxchgb %2, %1;"
                       "sete %0"
                       : "=q"(ret), "=m"(*ptr)
                       : "r"(new), "m"(ptr), "a"(old)
                       : "memory");
  return ret;
}