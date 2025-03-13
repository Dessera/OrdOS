
#include "kernel/memory/gdt.h"
#include "kernel/config/memory.h"
#include "kernel/utils/asm.h"

SECTION(".ldrdata")
struct gdt_desc gdt[MEM_GDT_MAX_ENTRIES] = {
  GDT_DESC_NULL(),  // Null descriptor
  GDT_DESC_KCODE(), // Kernel code segment
  GDT_DESC_KDATA(), // Kernel data segment
  GDT_DESC_VIDEO(), // Video memory segment
  GDT_DESC_NULL(),  // TSS placeholder
  GDT_DESC_UCODE(), // User code segment
  GDT_DESC_UDATA()  // User data segment
};
