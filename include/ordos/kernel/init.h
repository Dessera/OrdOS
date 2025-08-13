/**
 * @file init.h
 * @author Dessera (dessera@qq.com)
 * @brief Common kernel initialization info.
 * @version 0.1.0
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/kernel/config.h"
#include "ordos/kernel/mem/map.h"
#include "ordos/lib/types.h" // IWYU pragma: keep

/**
 * @brief Init information for kernel.
 *
 */
struct init_info
{
  // memory.
  struct mmap_entry mmap[ORDOS_INIT_MMAP_LENGTH];
  size_t mmap_cnt;
  uintptr_t mstart;
  uintptr_t mend;

  // error when initialize.
  const char* error;
};

extern struct init_info __init;
