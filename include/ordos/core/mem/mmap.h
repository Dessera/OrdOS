#pragma once

#include "ordos/lib/types.h"

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
 * @brief Memory map entry.
 *
 */
struct mmap_entry
{
  u64 addr;
  u64 len;
  enum mmap_type type;
};
