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
#include "ordos/kernel/module/version.h"
#include "ordos/lib/common.h"
#include "ordos/lib/list_head.h"
#include "ordos/lib/types.h"

#define __module_init(mentry, mexit, mflag, mname, mversion, ...)              \
  __used static const char* __##mname##_deps[] = { __VA_ARGS__ };              \
  __section(".data.module")                                                    \
    __used static struct module __##mname = { .name = #mname,                  \
                                              .version = mversion,             \
                                              .deps = __##mname##_deps,        \
                                              .flag = mflag,                   \
                                              .entry = mentry,                 \
                                              .exit = mexit,                   \
                                              .magic = ORDOS_MODULE_MAGIC };

/**
 * @brief Util to define a kernel module.
 *
 */
#define module_init(mentry, mexit, mflag, mname, mversion, ...)                \
  __module_init(mentry, mexit, mflag, mname, mversion, ##__VA_ARGS__, 0)

#define module_init_noexit(mentry, mflag, mname, mversion, ...)                \
  module_init(                                                                 \
    mentry, __module_default_exit, mflag, mname, mversion, ##__VA_ARGS__)

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

enum module_flag
{
  MOD_NOLOAD = 0,
  MOD_AUTOLOAD = 1,
};

/**
 * @brief Kernel module type.
 *
 */
struct module
{
  const char* name;
  const char* version;
  const char** deps;
  enum module_flag flag;
  module_entry_t entry;
  module_exit_t exit;
  u32 magic;
};

/**
 * @brief Loaded kernel module type.
 *
 */
struct loaded_module
{
  const char* name;
  struct version version;
  struct list_head node;
  struct loaded_module** deps;
  size_t deps_cnt;
  size_t refs_cnt;
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
 * @brief Load a module.
 *
 * @param name Module name.
 * @return int Load status.
 */
int
load_module(const char* name);

/**
 * @brief Unload a module.
 *
 * @param name Module name.
 */
void
unload_module(const char* name);

/**
 * @brief Find loaded module.
 *
 * @param name Module name.
 * @return struct loaded_module* Module pointer.
 */
struct loaded_module*
find_module(const char* name);

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
