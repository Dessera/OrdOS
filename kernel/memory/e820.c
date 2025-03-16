#include "kernel/memory/e820.h"

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