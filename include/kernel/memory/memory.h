#pragma once

#ifndef __ASM__
#include "lib/types.h"
#endif

#include "kernel/config/memory.h"

#ifndef __ASM__
#define MEM_GET_WITH_OFFSET(item, offset)                                      \
  (typeof(&item))((void*)((u32)(&item) + (offset)))
#else
#define MEM_GET_WITH_OFFSET(item, offset) item + offset
#endif

#define MEM_KERNEL_VADDR(item) item + MEM_KERNEL_VSTART
#define MEM_KERNEL_PADDR(item) item - MEM_KERNEL_VSTART

#define MEM_GET_WITH_KERNEL_VSTART(item)                                       \
  MEM_GET_WITH_OFFSET(item, MEM_KERNEL_VSTART)

#define MEM_GET_WITH_KERNEL_NVSTART(item)                                      \
  MEM_GET_WITH_OFFSET(item, -MEM_KERNEL_VSTART)

#ifndef __ASM__

void
init_memory(void);

#endif