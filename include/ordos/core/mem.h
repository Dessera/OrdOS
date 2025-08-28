/**
 * @file mem.h
 * @author Dessera (dessera@qq.com)
 * @brief Memory utils.
 * @version 0.1.0
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/core/mem/sslab.h"
#include "ordos/lib/common.h" // IWYU pragma: keep
#include "ordos/lib/types.h"  // IWYU pragma: keep

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
