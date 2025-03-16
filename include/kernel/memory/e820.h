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