#include "ordos/kernel/module.h"
#include "ordos/kernel/compiler.h"
#include "ordos/kernel/config.h"
#include "ordos/kernel/error.h"
#include "ordos/kernel/logging.h"
#include "ordos/lib/common.h"
#include "ordos/lib/string.h" // IWYU pragma: keep
#include "ordos/lib/types.h"  // IWYU pragma: keep

struct module* __mods;

size_t __mods_cnt = 0;

void
__unload_module(struct module* mod)
{
  if (mod->magic != ORDOS_MODULE_MAGIC) {
    kwarn("Module: Invalid module magic");
    return;
  }

  if (mod->refs_cnt == 0) {
    return;
  }

  --mod->refs_cnt;
  if (mod->refs_cnt != 0) {
    return;
  }

  mod->exit(mod);

  for (size_t i = 0; i < mod->deps_cnt; ++i) {
    if (mod->deps[i] == NULL) {
      break;
    }

    __unload_module(mod->deps[i]);
  }
}

int
__load_module(struct module* mod)
{
  if (mod->magic != ORDOS_MODULE_MAGIC) {
    kwarn("Module: Module magic mismatched");
    return E_LOAD;
  }

  if (mod->refs_cnt != 0) {
    goto load_success;
  }

  size_t loaded_deps_cnt = 0;
  for (size_t i = 0; i < mod->deps_cnt; ++i) {
    if (__load_module(mod->deps[i]) != E_SUCCESS) {
      goto deps_failed;
    }

    ++loaded_deps_cnt;
  }

  if (mod->entry(mod) != E_SUCCESS) {
    kwarn("Module: Unable to load %s because init failed", mod->name);
    goto init_failed;
  }

load_success:
  ++mod->refs_cnt;
  return E_SUCCESS;

init_failed:
deps_failed:
  for (size_t i = 0; i < loaded_deps_cnt; ++i) {
    if (mod->deps[i] == NULL) {
      break;
    }

    __unload_module(mod->deps[i]);
  }

  return E_LOAD;
}

void
init_module(void)
{
  void* mptr = (void*)compiler_kernel_module_start_vaddr();
  void* mptr_end = (void*)compiler_kernel_module_end_vaddr();

  __mods = mptr;

  while (mptr < mptr_end) {
    ++__mods_cnt;
    mptr = poffset(mptr, (sizeof(struct module)));
  }
}

struct module*
load_module(const char* name)
{
  struct module* mod = find_module(name);
  if (mod == NULL) {
    kwarn("Module: No module named %s in modules list", name);
    return NULL;
  }

  if (__load_module(mod) == E_SUCCESS) {
    return mod;
  }

  return NULL;
}

void
unload_module(const char* name)
{
  struct module* mod = find_module(name);
  if (mod == NULL) {
    kwarn("Module: No module named %s in modules list", name);
    return;
  }

  __unload_module(mod);
}

struct module*
find_module(const char* name)
{
  for (size_t i = 0; i < __mods_cnt; ++i) {
    if (strcmp(__mods[i].name, name) == 0) {
      return &__mods[i];
    }
  }

  return NULL;
}

size_t
autoload_module(int flags)
{
  int req = mask_flags(flags, MOD_AUTOLOAD | MOD_NOLOAD);
  size_t res = 0;

  for (size_t i = 0; i < __mods_cnt; ++i) {
    if (mask_flags(__mods[i].flag, MOD_AUTOLOAD | MOD_NOLOAD) == req) {
      if (has_flags(__mods[i].flag, MOD_AUTOLOAD) && __mods[i].refs_cnt == 0 &&
          __load_module(&__mods[i]) == E_SUCCESS) {
        ++res;
      }
    }
  }

  return res;
}

void
__module_default_exit(struct module* mod)
{
  kpanic("Module: Cannot unload module %s because it's declared with noexit",
         mod->name);
}
