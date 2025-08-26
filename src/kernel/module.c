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

size_t
__find_raw_module_deps_cnt(struct module* mod)
{
  const char** pdep = mod->deps;
  size_t deps_cnt = 0;
  while (*pdep != NULL) {
    ++deps_cnt;
    ++pdep;
  }

  return deps_cnt;
}

void
__unload_module(struct loaded_module* lmod)
{
  --lmod->refs_cnt;
  if (lmod->refs_cnt > 0) {
    return;
  }

  list_del(&lmod->node);
  --__mods_cnt;

  for (size_t i = 0; i < lmod->deps_cnt; ++i) {
    __unload_module(lmod->deps[i]);
  }

  kfree(lmod->deps);
  kfree(lmod);
}

static struct loaded_module*
__load_module(struct module* mod)
{
  struct loaded_module* lmod = find_module(mod->name);
  if (lmod != NULL) {
    goto load_success;
  }

  lmod = kmalloc(sizeof(struct loaded_module));
  if (lmod == NULL) {
    kwarn("Module: Unable to load module %s because alloc failed", mod->name);
    return NULL;
  }

  if (version_parse(&lmod->version, mod->version) != E_SUCCESS) {
    kwarn("Module: Unable to load module %s because version parse failed",
          mod->name);
    goto version_failed;
  }

  size_t deps_cnt = __find_raw_module_deps_cnt(mod);
  if (deps_cnt != 0) {
    lmod->deps = kmalloc(sizeof(struct loaded_module*) * deps_cnt);
    if (lmod->deps == NULL) {
      kwarn("Module: Unable to load module %s because depes alloc failed",
            mod->name);
      goto deps_alloc_failed;
    }
  }

  struct dependency dep;
  for (size_t i = 0; i < deps_cnt; ++i) {
    if (dependency_parse(&dep, mod->deps[i]) != E_SUCCESS) {
      kwarn("Module: Unable to load module %s because deps parse failed",
            mod->name);
      goto deps_failed;
    }

    struct module* mdep = __find_raw_module(dep.name);
    if (mdep == NULL) {
      kwarn("Module: Unable to load module %s because missing dep %s",
            mod->name,
            dep.name);
      goto deps_failed;
    }

    struct loaded_module* ldep = __load_module(mdep);
    if (ldep == NULL) {
      goto deps_failed;
    }

    if (version_compare(&ldep->version, &dep.version) < 0) {
      kwarn("Module: Unable to load module %s because dep %s is unsatisfied",
            mod->name,
            dep.name);
      goto deps_failed;
    }

    lmod->deps[i] = ldep;
    ++lmod->deps_cnt;
  }

  lmod->name = mod->name;
  lmod->entry = mod->entry;
  lmod->exit = mod->exit;

  if (lmod->entry(lmod) != E_SUCCESS) {
    kwarn("Module: Unable to load module %s because init failed", mod->name);
    goto init_failed;
  }

  list_add(&lmod->node, &__mods_list);
  ++__mods_cnt;

load_success:
  ++lmod->refs_cnt;
  return lmod;

init_failed:
deps_failed:
  for (size_t i = 0; i < lmod->deps_cnt; ++i) {
    __unload_module(lmod->deps[i]);
  }
  kfree(lmod->deps);
deps_alloc_failed:
version_failed:
  kfree(lmod);
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
      kwarn("Module: Module magic mismatched");
      continue;
    }

    if (__raw_mods[i].flag == MOD_AUTOLOAD) {
      __load_module(&__raw_mods[i]);
    }
  }

  kinfo("Module: %u loaded", __mods_cnt);
}

int
load_module(const char* name)
{
  struct module* mod = __find_raw_module(name);
  if (mod == NULL) {
    kwarn("Module: No module named %s in modules list", name);
    return E_NOTFOUND;
  }

  struct loaded_module* lmod = __load_module(mod);
  if (lmod == NULL) {
    return E_LOAD;
  }

  return E_SUCCESS;
}

void
unload_module(const char* name)
{
  struct loaded_module* lmod = find_module(name);
  if (lmod == NULL) {
    kwarn("Module: No module named %s in modules list", name);
    return;
  }

  __unload_module(lmod);
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

struct loaded_module*
find_module_dep(struct loaded_module* mod, const char* name)
{
  for (size_t i = 0; i < mod->deps_cnt; ++i) {
    if (strcmp(mod->deps[i]->name, name) == 0) {
      return mod->deps[i];
    }
  }

  return NULL;
}

void
__module_default_exit(struct loaded_module* mod)
{
  kpanic("Module: Cannot unload module %s because it's declared with noexit",
         mod->name);
}
