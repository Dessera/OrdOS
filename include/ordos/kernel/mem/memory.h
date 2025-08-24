/**
 * @file memory.h
 * @author Dessera (dessera@qq.com)
 * @brief Memory utils.
 * @version 0.1.0
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/kernel/mem/sslab/sslab.h"
#include "ordos/lib/common.h" // IWYU pragma: keep
#include "ordos/lib/types.h"  // IWYU pragma: keep

#define MEM_TYPE_HIGH_START 0x30000000
#define MEM_TYPE_NORMAL_START 0x01000000
#define MEM_TYPE_DMA_START 0x00000000

/**
 * @brief Memory map types.
 *
 */
enum mmap_type
{
  PMEM_AVAILABLE = 1,
  PMEM_RESERVED = 2,
  PMEM_ACPI_RECLAIMABLE = 3,
  PMEM_NVS = 4,
  PMEM_BADRAM = 5,
};

/**
 * @brief Memory types.
 *
 */
enum mem_type
{
  MEM_DMA,
  MEM_NORMAL,
  MEM_HIGH,
};

/**
 * @brief Memory map entry.
 *
 */
struct mmap_entry
{
  u64 addr;
  u64 len;
  enum mmap_type type;
};

/**
 * @brief Initialize the memory management system.
 */
void
init_memory(void);

/**
 * @brief Allocate a block of memory (sslab wrapper).
 *
 * @param size Size of the block to allocate (pass 2 ^ n for best performance).
 * @return void* Pointer to the block allocated.
 */
__inline static void*
kmalloc(size_t size)
{
  return sslab_global_alloc(size);
}

/**
 * @brief Free a block of memory (sslab wrapper).
 *
 * @param obj Pointer to the block to free.
 */
__inline static void
kfree(void* obj)
{
  return sslab_global_free(obj);
}
