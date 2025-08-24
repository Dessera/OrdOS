#include "ordos/kernel/module.h"
#include "ordos/kernel/compiler.h"
#include "ordos/kernel/config.h"
#include "ordos/kernel/logging.h"
#include "ordos/lib/common.h"
#include "ordos/lib/types.h" // IWYU pragma: keep

void
init_module(void)
{
  void* mptr = (void*)compiler_kernel_module_start_vaddr();
  void* mptr_end = (void*)compiler_kernel_module_end_vaddr();

  size_t mod_cnt = 0;
  while (mptr < mptr_end) {
    struct module* mod = mptr;

    if (mod->magic != ORDOS_MODULE_MAGIC) {
      kwarn("Module: Invalid module structure");
      goto next_mod;
    }

    load_module(mod);
    ++mod_cnt;

  next_mod:
    mptr = poffset(mptr, (sizeof(struct module)));
  }

  kinfo("Module: %u loaded", mod_cnt);
}

void
load_module(struct module* mod)
{
  mod->entry();
}

void
unload_module(struct module* mod)
{
  mod->exit();
}
