#include "kernel/info.h"
#include "kernel/interrupt/idt.h"
#include "kernel/types.h"
#include "kernel/utils/print.h"

void
kmain(void);

static void
kinit(void)
{
  init_idt();
}

void
kmain(void)
{
  kscrclear();

  kputs(KFULLNAME_STR);
  kputs(" STEP 3\n");

  kinit();

  while (true)
    ;
}