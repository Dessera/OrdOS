#include "ordos/core/intr/idt.h"
#include "ordos/config.h"
#include "ordos/core/mem/gdt.h"
#include "ordos/lib/logging.h"

extern void* _asm_intr_vecs[ORDOS_INTR_IDT_DESC_CNT];

static struct idt __idt[ORDOS_INTR_IDT_DESC_CNT];

void
init_idt(void)
{
  // other interrupts
  for (u16 i = 0; i < ORDOS_INTR_IDT_DESC_CNT; i++) {
    id_desc_init(__idt[i],
                 _asm_intr_vecs[i],
                 gdt_sel_kcode(),
                 IDT_PRESENT | IDT_KERNEL | IDT_INTRGATE);
  }

  // syscall
  id_desc_init(__idt[ORDOS_INTR_SYSCALL],
               _asm_intr_vecs[ORDOS_INTR_SYSCALL],
               gdt_sel_kcode(),
               IDT_PRESENT | IDT_USER | IDT_INTRGATE);

  lidt(idt_get_ptr(__idt, ORDOS_INTR_IDT_DESC_CNT));

  kdebug("IDT: %p", __idt);
}
