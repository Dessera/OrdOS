/**
 * @file module.h
 * @author Dessera (dessera@qq.com)
 * @brief Kernel module utils.
 * @version 0.1.0
 * @date 2025-08-24
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/kernel/config.h"
#include "ordos/kernel/module/deps.h"
#include "ordos/kernel/module/version.h"
#include "ordos/lib/common.h"
#include "ordos/lib/list_head.h"
#include "ordos/lib/types.h"

#define __module_init(mentry, mexit, mname, mversion, ...)                     \
  __used static const char* __##mname##_deps[] = { __VA_ARGS__ };              \
  __section(".data.module")                                                    \
    __used static struct module __##mname = { .name = #mname,                  \
                                              .version = mversion,             \
                                              .deps = __##mname##_deps,        \
                                              .entry = mentry,                 \
                                              .exit = mexit,                   \
                                              .magic = ORDOS_MODULE_MAGIC };

/**
 * @brief Util to define a kernel module.
 *
 */
#define module_init(mentry, mexit, mname, mversion, ...)                       \
  __module_init(mentry, mexit, mname, mversion, ##__VA_ARGS__, 0)

#define module_init_noexit(mentry, mname, mversion, ...)                       \
  module_init(mentry, __module_default_exit, mname, mversion, ##__VA_ARGS__)

struct loaded_module;

/**
 * @brief Kernel module entry function type.
 *
 */
typedef int (*module_entry_t)(struct loaded_module* mod);

/**
 * @brief Kernel module exit function type.
 *
 */
typedef void (*module_exit_t)(struct loaded_module* mod);

/**
 * @brief Kernel module type.
 *
 */
struct module
{
  const char* name;
  const char* version;
  const char** deps;
  module_entry_t entry;
  module_exit_t exit;
  u32 magic;
};

struct loaded_module
{
  const char* name;
  struct version version;
  struct dependency* deps;
  size_t deps_cnt;
  struct list_head node;
  module_entry_t entry;
  module_exit_t exit;
};

/**
 * @brief Initialize all modules.
 *
 */
void
init_module(void);

/**
 * @brief Load module to kernel.
 *
 * @param mod Raw module pointer.
 * @return Loaded module.
 */
struct loaded_module*
load_module(struct module* mod);

/**
 * @brief Unload module to kernel.
 *
 * @param mod Loaded module pointer.
 */
void
unload_module(struct loaded_module* mod);

/**
 * @brief Find loaded module.
 *
 * @param name Module name.
 * @return struct loaded_module* Module pointer.
 */
struct loaded_module*
find_module(const char* name);

/**
 * @brief Check if module has dependency.
 *
 * @param mod Loaded module.
 * @param name Dependency name.
 * @return true If module has the dependency, otherwise false.
 */
bool
module_has_dep(struct loaded_module* mod, const char* name);

/**
 * @brief Find loaded module dependency.
 *
 * @param mod Loaded module.
 * @param name Dependency name.
 * @return struct loaded_module* Dependency.
 */
struct loaded_module*
find_module_dep(struct loaded_module* mod, const char* name);

/**
 * @brief Default exit function for modules that cannot be unloaded.
 *
 * @param mod Loaded module.
 */
void
__module_default_exit(struct loaded_module* mod);
