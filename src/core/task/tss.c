#include "ordos/config.h"
#include "ordos/core/mem/gdt.h"
#include "ordos/core/task/context.h"
#include "ordos/lib/common.h"
#include "ordos/lib/types.h"

static struct tss_context __tss_ctx = { 0, 0, gdt_sel_kdata(),
                                        0, 0, 0,
                                        0, 0, 0,
                                        0, 0, 0,
                                        0, 0, 0,
                                        0, 0, 0,
                                        0, 0, 0,
                                        0, 0, 0,
                                        0, 0, sizeof(struct tss_context) };

void
init_tss(void)
{
  struct gdt tss_desc = gdt_desc_tss(&__tss_ctx);
  __gdt[GDT_TSS_INDEX] = tss_desc;

  u64 gdt_ptr = gdt_create_ptr(__gdt, ORDOS_MEM_GDT_DESC_CNT);
  asm_exec("lgdt %0" : : "m"(gdt_ptr));
  asm_exec("ltr %w0" : : "r"(gdt_sel_tss()));
}

void
tss_update_esp(struct task* task)
{
  __tss_ctx.esp0 =
    (u32*)align_up((uintptr_t)task->stack, ORDOS_KERNEL_PAGE_SIZE);
}
