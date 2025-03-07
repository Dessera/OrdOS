#include "kernel/device/sys_clk.h"
#include "kernel/device/vga.h"
#include "kernel/info.h"
#include "kernel/interrupt/intr.h"
#include "kernel/memory/mm.h"
#include "kernel/types.h"
#include "kernel/utils/print.h"

void
kmain(void);

static void
kinit(void)
{
  init_vga();

  kputs(KFULLNAME_STR);
  kputs(" STEP 3\n");

  init_intr();
  init_sys_clk();
  init_mm();
}

void
kmain(void)
{
  kinit();

  while (true)
    ;
}