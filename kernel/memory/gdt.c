#include "kernel/memory/gdt.h"
#include "kernel/config/memory.h"

struct gdt_desc gdt[MEM_GDT_SIZE] = {
  GDT_DESC_NULL(), GDT_DESC_TCODE(), GDT_DESC_TDATA(), GDT_DESC_VIDEO(),
  GDT_DESC_NULL(), GDT_DESC_UCODE(), GDT_DESC_UDATA(), GDT_DESC_NULL(),
};