#include "ordos/core/mem.h"
#include "ordos/core/mem/buddy.h"
#include "ordos/core/mem/buddy/page.h"
#include "ordos/core/mem/buddy/zone.h"
#include "ordos/core/mem/sslab.h"
#include "ordos/core/mem/vpage.h"
#include "ordos/init.h"
#include "ordos/lib/common.h"
#include "ordos/lib/error.h"
#include "ordos/lib/logging.h"
#include "ordos/lib/types.h" // IWYU pragma: keep
#include "ordos/module.h"

/**
 * @brief Unload top pages.
 *
 */
static void
__init_vpage(void)
{
  pde_t* pd = paccess(__init.kernel_pd);

  for (size_t i = 0; i < __init.kernel_pde_cnt; ++i) {
    pd[i] = 0;
  }
}

int
mem_entry(struct module* mod)
{
  init_buddy();
  init_sslab();

  __init_vpage();

  minfo(mod,
        "DMA memory    %u MB, %u pages",
        page_size(__zones[MEM_DMA].pg_free, MBYTES),
        __zones[MEM_DMA].pg_free);
  minfo(mod,
        "NORMAL memory %u MB, %u pages",
        page_size(__zones[MEM_NORMAL].pg_free, MBYTES),
        __zones[MEM_NORMAL].pg_free);
  minfo(mod,
        "HIGH memory   %u MB, %u pages",
        page_size(__zones[MEM_HIGH].pg_free, MBYTES),
        __zones[MEM_HIGH].pg_free);

  return E_SUCCESS;
}

module_init_noexit(sys_mem, MOD_CORE | MOD_AUTOLOAD, mem_entry);
