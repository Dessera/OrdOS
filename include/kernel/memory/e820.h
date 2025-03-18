#pragma once

#include "lib/types.h"

enum e820_type : u32
{
  E820_TYPE_RAM = 1,
  E820_TYPE_RESERVED,
  E820_TYPE_ACPI,
  E820_TYPE_NVS,
  E820_TYPE_UNUSABLE,
  E820_TYPE_PMEM,
};

struct e820_entry
{
  u64 base;
  u64 length;
  enum e820_type type;
} __attribute__((packed));

const char*
e820_type_to_string(enum e820_type type);

/**
 * @brief Get the e820 entries (from loader section)
 *
 * @return struct e820_entry* pointer to the entries
 */
struct e820_entry*
e820_get_entries(void);

/**
 * @brief Get the number of e820 entries
 *
 * @return size_t number of entries (u16 max)
 */
size_t
e820_get_entries_cnt(void);

/**
 * @brief Get the total memory size
 *
 * @return uintptr_t total memory size
 */
uintptr_t
e820_get_memory_size(void);
