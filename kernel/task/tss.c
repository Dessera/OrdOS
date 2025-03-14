#include "kernel/task/tss.h"
#include "kernel/config/memory.h"
#include "kernel/memory/gdt.h"
#include "kernel/task/context.h"
#include "kernel/types.h"

#define MK_TSS_DESC(ctx)                                                       \
  GDT_DESC(                                                                    \
    sizeof(struct tss_context), (u32)ctx, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0)

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
init_user_tss(void)
{
  struct gdt_desc tss_desc = MK_TSS_DESC(&__tss_ctx);
  gdt[GDT_TSS_INDEX] = tss_desc;

  u64 gdt_ptr = GDT_GET_PTR(MEM_GDT_MAX_ENTRIES, (u32)gdt + MEM_KERNEL_VSTART);
  __asm__ __volatile__("lgdt %0" : : "m"(gdt_ptr));
  __asm__ __volatile__("ltr %w0" : : "r"(GDT_TSS_SELECTOR));
}

void
tss_update_esp(struct task* task)
{
  __tss_ctx.esp0 = (u32*)(((u32)task) + MEM_PAGE_SIZE);
}