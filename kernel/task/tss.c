#include "kernel/task/tss.h"
#include "kernel/config/memory.h"
#include "kernel/memory/gdt.h"
#include "kernel/task/context.h"
#include "lib/types.h"

static struct tss_context __tss_ctx = { 0, 0, GDT_KSTACK_SELECTOR,
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
  gdt[GDT_TSS_INDEX] = GDT_TSS_DESC(&__tss_ctx);

  u64 gdt_ptr = gdt_get_ptr(gdt, ARRAY_SIZE(gdt));
  __asm__ __volatile__("lgdt %0" : : "m"(gdt_ptr));
  __asm__ __volatile__("ltr %w0" : : "r"(GDT_TSS_SELECTOR));
}

void
tss_update_esp(struct task* task)
{
  __tss_ctx.esp0 =
    (u32*)(((uintptr_t)task->stack + MEM_PAGE_SIZE) & ~(MEM_PAGE_SIZE - 1));
}