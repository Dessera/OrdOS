#pragma once

#include "kernel/types.h"

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