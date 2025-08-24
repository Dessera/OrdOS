#include "ordos/kernel/init.h"
#include "ordos/kernel/logging.h"
#include "ordos/kernel/mem/buddy/buddy.h"
#include "ordos/kernel/mem/sslab/sslab.h"
#include "ordos/kernel/mem/vpage.h"
#include "ordos/kernel/utils.h"
#include "ordos/lib/types.h" // IWYU pragma: keep

static void
__init_vpage(void)
{
  pde_t* pd = paccess(__init.kernel_pd);

  for (size_t i = 0; i < __init.kernel_pde_cnt; ++i) {
    pd[i] = 0;
  }
}

void
init_memory(void)
{
  kinfo("Initializing memory management subsystem");

  init_buddy();
  init_sslab();

  __init_vpage();
}
