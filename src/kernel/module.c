#include "ordos/kernel/module.h"
#include "ordos/kernel/compiler.h"
#include "ordos/kernel/config.h"
#include "ordos/kernel/error.h"
#include "ordos/kernel/logging.h"
#include "ordos/kernel/mem/memory.h"
#include "ordos/kernel/module/deps.h"
#include "ordos/kernel/module/version.h"
#include "ordos/lib/common.h"
#include "ordos/lib/list_head.h"
#include "ordos/lib/string.h" // IWYU pragma: keep
#include "ordos/lib/types.h"  // IWYU pragma: keep

struct list_head __mods_list;

size_t __mods_cnt = 0;

struct module* __raw_mods;

size_t __raw_mods_cnt = 0;

struct loaded_module*
__load_module(struct module* mod)
{
  struct loaded_module* lmod = kmalloc(sizeof(struct loaded_module));
  if (lmod == NULL) {
    kwarn("Module: Unable to load module %s because alloc failed", mod->name);
    return NULL;
  }

  lmod->name = mod->name;

  if (version_parse(&lmod->version, mod->version) != E_SUCCESS) {
    kwarn("Module: Unable to load module %s because version parse failed",
          mod->name);
    goto version_failed;
  }

  const char** pdep = mod->deps;
  while (*pdep != NULL) {
    ++lmod->deps_cnt;
    ++pdep;
  }

  lmod->deps = kmalloc(sizeof(struct dependency) * lmod->deps_cnt);
  if (lmod->deps == NULL) {
    kwarn("Module: Unable to load module %s because depes alloc failed",
          mod->name);
    goto deps_alloc_failed;
  }

  for (size_t i = 0; i < lmod->deps_cnt; ++i) {
    if (dependency_parse(&lmod->deps[i], mod->deps[i]) != E_SUCCESS) {
      kwarn("Module: Unable to load module %s because deps parse failed",
            mod->name);
      goto deps_failed;
    }
  }

  lmod->entry = mod->entry;
  lmod->exit = mod->exit;

  return lmod;

deps_failed:
  kfree(lmod->deps);
deps_alloc_failed:
version_failed:
  kfree(lmod);
  return NULL;
}

void
__unload_module(struct loaded_module* lmod)
{
  kfree(lmod->deps);
  kfree(lmod);
}

struct module*
__find_raw_module(const char* name)
{
  for (size_t i = 0; i < __raw_mods_cnt; ++i) {
    if (__raw_mods[i].magic != ORDOS_MODULE_MAGIC) {
      continue;
    }

    if (strcmp(__raw_mods[i].name, name) != 0) {
      continue;
    }

    return &__raw_mods[i];
  }

  return NULL;
}

void
init_module(void)
{
  list_init(&__mods_list);

  void* mptr = (void*)compiler_kernel_module_start_vaddr();
  void* mptr_end = (void*)compiler_kernel_module_end_vaddr();

  __raw_mods = mptr;

  while (mptr < mptr_end) {
    ++__raw_mods_cnt;
    mptr = poffset(mptr, (sizeof(struct module)));
  }

  for (size_t i = 0; i < __raw_mods_cnt; ++i) {

    if (__raw_mods[i].magic != ORDOS_MODULE_MAGIC) {
      kwarn("Module: Unable to load one module because magic mismatched");
      continue;
    }

    load_module(&__raw_mods[i]);
  }

  kinfo("Module: %u loaded", __mods_cnt);
}

struct loaded_module*
load_module(struct module* mod)
{
  // if module is loaded, return it.
  // do not support load same module which has different version.
  struct loaded_module* mod_entry = find_module(mod->name);
  if (mod_entry != NULL) {
    return mod_entry;
  }

  // load module into memory.
  struct loaded_module* lmod = __load_module(mod);
  if (lmod == NULL) {
    return NULL;
  }

  // resolve dependencies, if failed, do **not** rollback.
  for (size_t i = 0; i < lmod->deps_cnt; ++i) {
    struct module* dep = __find_raw_module(lmod->deps[i].name);
    if (dep == NULL) {
      kwarn("Module: Unable to load module %s because missing dep %s",
            mod->name,
            lmod->deps[i].name);
      goto dep_check_failed;
    }

    struct loaded_module* ldep = load_module(dep);
    if (ldep == NULL) {
      goto dep_check_failed;
    }

    if (version_compare(&ldep->version, &lmod->deps[i].version) < 0) {
      kwarn("Module: Unable to load module %s because dep %s is unsatisfied",
            mod->name,
            lmod->deps[i].name);
      goto dep_check_failed;
    }
  }

  // call entry.
  if (lmod->entry(lmod) != E_SUCCESS) {
    kwarn("Module: Unable to load module %s because init failed", mod->name);
    goto init_failed;
  }

  list_add(&lmod->node, &__mods_list);
  ++__mods_cnt;

  return lmod;

init_failed:
dep_check_failed:
  __unload_module(lmod);
  return NULL;
}

// TODO: impl
void
unload_module(struct loaded_module* mod)
{
  (void)mod;
  // unload module depends on this.
  // struct list_head* entry = NULL;
  // list_for_each(entry, &__mods_list)
  // {
  //   struct loaded_module* mod_entry =
  //     list_entry(entry, struct loaded_module, node);
  //   if (module_has_dep(mod_entry, mod->name)) {
  //     unload_module(mod_entry);
  //   }
  // }

  // __unload_module(mod);
}

struct loaded_module*
find_module(const char* name)
{
  struct list_head* entry = NULL;
  list_for_each(entry, &__mods_list)
  {
    struct loaded_module* mod_entry =
      list_entry(entry, struct loaded_module, node);
    if (strcmp(mod_entry->name, name) == 0) {
      // module has loaded
      return mod_entry;
    }
  }

  return NULL;
}

bool
module_has_dep(struct loaded_module* mod, const char* name)
{
  for (size_t i = 0; i < mod->deps_cnt; ++i) {
    if (strcmp(mod->deps[i].name, name) == 0) {
      return true;
    }
  }

  return false;
}

struct loaded_module*
find_module_dep(struct loaded_module* mod, const char* name)
{
  if (module_has_dep(mod, name)) {
    return find_module(name);
  }

  return NULL;
}

void
__module_default_exit(struct loaded_module* mod)
{
  kpanic("Module: Cannot unload module %s because it's declared by noexit",
         mod->name);
}
