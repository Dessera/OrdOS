#include "kernel/types.h"
#include "kernel/utils/print.h"

void
kmain(void);

void
kmain(void)
{
  kscrclear();

  kputs("Hello, world!\n");

  while (true)
    ;
}