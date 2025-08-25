#include "ordos/kernel/error.h"
#include "ordos/kernel/module.h"

int
timer_entry(struct loaded_module* mod)
{
  (void)mod;

  return E_SUCCESS;
}

module_init_noexit(timer_entry, sys_timer, "0.1.0")
