#include "kernel/task/tss.h"
#include "kernel/config/memory.h"
#include "kernel/memory/gdt.h"
#include "kernel/task/context.h"
#include "kernel/types.h"

#define MK_TSS_DESC(ctx)                                                       \
  GDT_DESC(                                                                    \
    sizeof(struct tss_context), (u32)ctx, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0)

#define MK_GDT_PTR(size, addr)                                                 \
  ((sizeof(struct gdt_desc) * (size) - 1) |                                    \
   ((u64)((u32)(addr) + MEM_KERNEL_VSTART) << 16))

extern struct gdt_desc _asm_gdt_table[];

// only refer to the initial gdt size
// (invalid when tss is loaded)
extern u16 _asm_gdt_size;

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
  u16 gdt_size = _asm_gdt_size;

  _asm_gdt_table[gdt_size++] = MK_TSS_DESC(&__tss_ctx);

  _asm_gdt_table[gdt_size++] = GDT_DESC_UCODE();
  _asm_gdt_table[gdt_size++] = GDT_DESC_UDATA();

  u64 gdt_ptr = MK_GDT_PTR(gdt_size, _asm_gdt_table);
  __asm__ __volatile__("lgdt %0" : : "m"(gdt_ptr));
  __asm__ __volatile__("ltr %w0" : : "r"(GDT_TSS_SELECTOR));
}

void
tss_update_esp(struct task* task)
{
  __tss_ctx.esp0 = (u32*)(((u32)task) + MEM_PAGE_SIZE);
}