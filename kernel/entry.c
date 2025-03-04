#include "kernel/device/sys_clk.h"
#include "kernel/device/vga.h"
#include "kernel/info.h"
#include "kernel/interrupt/idt.h"
#include "kernel/memory/page.h"
#include "kernel/types.h"
#include "kernel/utils/asm.h"
#include "kernel/utils/print.h"

void
kmain(void);

static void
kinit(void)
{
  init_vga();

  kputs(KFULLNAME_STR);
  kputs(" STEP 3\n");

  init_idt();
  init_sys_clk();
  init_mm();
}

void
kmain(void)
{
  kinit();

  void* mm = alloc_page(1);
  kput_u32((u32)mm);
  kputchar('\n');

  void* mm2 = alloc_page(5);
  kput_u32((u32)mm2);
  kputchar('\n');

  void* mm3 = alloc_page(3);
  kput_u32((u32)mm3);
  kputchar('\n');

  while (true) {
    hlt();
  }
}