#include "kernel/assert.h"
#include "kernel/device/sys_clk.h"
#include "kernel/device/vga.h"
#include "kernel/info.h"
#include "kernel/interrupt/idt.h"
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

  init_idt();
  init_sys_clk();
}

void
kmain(void)
{
  kinit();

  while (true)
    ;
}