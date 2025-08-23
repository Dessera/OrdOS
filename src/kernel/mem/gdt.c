#include "ordos/kernel/mem/gdt.h"

struct gdt __gdt[GDT_DESC_LENGTH] = {
  gdt_desc_null(),  gdt_desc_kcode(), gdt_desc_kdata(),
  gdt_desc_ucode(), gdt_desc_udata(),
};
