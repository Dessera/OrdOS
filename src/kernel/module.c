#include "ordos/kernel/module.h"
#include "ordos/kernel/compiler.h"
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
    mod->deps[i] = NULL;
  }
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

  if (mod->refs_cnt != 0) {
    goto load_success;
  }

  for (size_t i = 0; i < mod->deps_cnt; ++i) {
    struct module* dep = load_module(mod->deps_name[i]);
    if (dep == NULL) {
      goto deps_failed;
    }

    mod->deps[i] = dep;
  }

  if (mod->entry(mod) != E_SUCCESS) {
    kwarn("Module: Unable to load %s because init failed", mod->name);
    goto init_failed;
  }

load_success:
  ++mod->refs_cnt;
  return mod;

init_failed:
deps_failed:
  for (size_t i = 0; i < mod->deps_cnt; ++i) {
    if (mod->deps[i] == NULL) {
      break;
    }

    __unload_module(mod->deps[i]);
    mod->deps[i] = NULL;
  }

  return NULL;
}

void
unload_module(const char* name)
{
  struct module* mod = find_module(name);
  if (mod == NULL || mod->refs_cnt != 0) {
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

struct module*
find_module_dep(struct module* mod, const char* name)
{
  for (size_t i = 0; i < mod->deps_cnt; ++i) {
    if (strcmp(mod->deps_name[i], name) == 0) {
      return mod->deps[i];
    }
  }

  return NULL;
}

void
__module_default_exit(struct module* mod)
{
  kpanic("Module: Cannot unload module %s because it's declared with noexit",
         mod->name);
}
