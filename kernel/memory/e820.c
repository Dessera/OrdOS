#include "kernel/memory/e820.h"
#include "kernel/memory/memory.h"
#include "lib/types.h"

extern u16 _asm_mem_nr;
extern struct e820_entry _asm_mem_zone;

const char*
e820_type_to_string(enum e820_type type)
{
  switch (type) {
    case E820_TYPE_RAM:
      return "RAM";
    case E820_TYPE_RESERVED:
      return "Reserved";
    case E820_TYPE_ACPI:
      return "ACPI";
    case E820_TYPE_NVS:
      return "NVS";
    case E820_TYPE_UNUSABLE:
      return "Unusable";
    case E820_TYPE_PMEM:
      return "Persistent Memory";
    default:
      return "Unknown";
  }
}

struct e820_entry*
e820_get_entries(void)
{
  return MEM_GET_WITH_KERNEL_VSTART(&_asm_mem_zone);
}

size_t
e820_get_entries_cnt(void)
{
  return (size_t)(*MEM_GET_WITH_KERNEL_VSTART(&_asm_mem_nr));
}

uintptr_t
e820_get_memory_size(void)
{
  struct e820_entry* mem_entries = e820_get_entries();
  size_t mem_cnt = e820_get_entries_cnt();

  uintptr_t mem_end = 0;
  for (size_t i = 0; i < mem_cnt; i++) {
    if (mem_entries[i].type != E820_TYPE_RAM) {
      continue;
    }

    u64 mem_size = mem_entries[i].base + mem_entries[i].length;
    if (mem_size > mem_end) {
      mem_end = mem_size;
    }
  }

  return mem_end;
}