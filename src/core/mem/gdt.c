#include "ordos/core/mem/gdt.h"
#include "ordos/config.h"

struct gdt __gdt[ORDOS_MEM_GDT_DESC_CNT] = {
  gdt_desc_null(),  gdt_desc_kcode(), gdt_desc_kdata(),
  gdt_desc_ucode(), gdt_desc_udata(),
};
