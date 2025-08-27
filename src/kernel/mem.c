#include "ordos/kernel/error.h"
#include "ordos/kernel/init.h"
#include "ordos/kernel/mem/vpage.h"
#include "ordos/kernel/module.h"
#include "ordos/kernel/utils.h"
#include "ordos/lib/types.h" // IWYU pragma: keep

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
  (void)mod;

  __init_vpage();

  return E_SUCCESS;
}

module_init_noexit(sys_mem,
                   MOD_COREMOD | MOD_AUTOLOAD,
                   mem_entry,
                   "sys_mem_sslab")
