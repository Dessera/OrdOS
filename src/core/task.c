#include "ordos/kernel/module.h"
#include "ordos/lib/error.h"

int
task_entry(struct module* mod)
{
  (void)mod;

  return E_SUCCESS;
}

module_dependency(sys_mem);
module_dependency(drv_pit);

module_init_noexit(sys_task,
                   MOD_COREMOD | MOD_AUTOLOAD,
                   task_entry,
                   sys_mem,
                   drv_pit);
