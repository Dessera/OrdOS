/**
 * @file map.h
 * @author Dessera (dessera@qq.com)
 * @brief Init memory info map.
 * @version 0.1.0
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/lib/types.h" // IWYU pragma: keep

/**
 * @brief Memory types.
 *
 */
enum mmap_type : u64
{
  PMEM_AVAILABLE = 1,
  PMEM_RESERVED = 2,
  PMEM_ACPI_RECLAIMABLE = 3,
  PMEM_NVS = 4,
  PMEM_BADRAM = 5,
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
