#include "kernel/memory/gdt.h"
#include "lib/asm.h"

SECTION(".ldrtext")
struct gdt_desc boot_gdt[MEM_BOOT_GDT_SIZE] = { GDT_DESC_NULL(),
                                                GDT_DESC_KCODE(),
                                                GDT_DESC_KDATA() };