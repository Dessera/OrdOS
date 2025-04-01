#pragma once

#include "kernel/config/memory.h"

#ifndef __ASSEMBLER__

#include "lib/types.h" // IWYU pragma: keep

/**
 * @brief Get the kernel virtual address of an address
 */
#define MEM_KERNEL_VADDR(item) (((uintptr_t)item) + MEM_KERNEL_VSTART)

/**
 * @brief Get the kernel physical address of an address
 */
#define MEM_KERNEL_PADDR(item) (((uintptr_t)item) - MEM_KERNEL_VSTART)

/**
 * @brief Get the item with a given offset (used to get item from other
 * segments because gdt and pagetables **DONOT** map memory under 1MB)
 */
#define MEM_GET_WITH_OFFSET(item, offset)                                      \
  ((typeof(item))((void*)((uintptr_t)(item) + (offset))))
#else
#define MEM_KERNEL_VADDR(item) ((item) + MEM_KERNEL_VSTART)
#define MEM_KERNEL_PADDR(item) ((item) - MEM_KERNEL_VSTART)

#define MEM_GET_WITH_OFFSET(item, offset) item + offset
#endif

#define MEM_GET_WITH_KERNEL_VSTART(item)                                       \
  MEM_GET_WITH_OFFSET(item, MEM_KERNEL_VSTART)

#define MEM_GET_WITH_KERNEL_NVSTART(item)                                      \
  MEM_GET_WITH_OFFSET(item, -MEM_KERNEL_VSTART)

#define MEM_TYPE_HIGH_START 0x30000000
#define MEM_TYPE_NORMAL_START 0x01000000
#define MEM_TYPE_DMA_START 0x00000000

#ifndef __ASSEMBLER__

#define KMALLOC_CLEANUP CLEAN_UP(kmcleanup)

/**
 * @brief Memory zone type
 */
ENUM_DECLARE(mem_type, MEM_ZONE_DMA, MEM_ZONE_NORMAL, MEM_ZONE_HIGH);

/**
 * @brief Initialize the memory management system
 */
void
init_memory(void);

/**
 * @brief Allocate a block of memory (sslab wrapper)
 *
 * @param size Size of the block to allocate (pass 2 ^ n for best performance)
 * @return void* Pointer to the block allocated
 */
void*
kmalloc(size_t size);

/**
 * @brief Free a block of memory (sslab wrapper)
 *
 * @param obj Pointer to the block to free
 */
void
kfree(void* obj);

static FORCE_INLINE void
kmcleanup(void** obj)
{
  kfree(*obj);
}

#endif