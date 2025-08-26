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
#include "ordos/lib/common.h"
#include "ordos/lib/types.h"

/**
 * @brief Util to define a kernel module.
 *
 */
#define module_init(mentry, mexit, mflag, mname, ...)                          \
  __used static const char* __##mname##_deps_name[] = { __VA_ARGS__ };         \
  __used static struct module*                                                 \
    __##mname##_deps[countof(__##mname##_deps_name)] = {};                     \
  __section(".data.module") __used static struct module __##mname = {          \
    .name = #mname,                                                            \
    .deps_name = __##mname##_deps_name,                                        \
    .deps = __##mname##_deps,                                                  \
    .deps_cnt = countof(__##mname##_deps_name),                                \
    .refs_cnt = 0,                                                             \
    .flag = mflag,                                                             \
    .entry = mentry,                                                           \
    .exit = mexit,                                                             \
    .magic = ORDOS_MODULE_MAGIC                                                \
  };

/**
 * @brief Util to define a noexit kernel module.
 *
 */
#define module_init_noexit(mentry, mflag, mname, ...)                          \
  module_init(mentry, __module_default_exit, mflag, mname, ##__VA_ARGS__)

struct module;

/**
 * @brief Kernel module entry function type.
 *
 */
typedef int (*module_entry_t)(struct module* mod);

/**
 * @brief Kernel module exit function type.
 *
 */
typedef void (*module_exit_t)(struct module* mod);

enum module_flag
{
  MOD_NOLOAD = 0x00,
  MOD_AUTOLOAD = 0x01,
  MOD_SYSMOD = 0x00,
  MOD_COREMOD = 0x02,
};

/**
 * @brief Kernel module type.
 *
 */
struct module
{
  const char* name;
  struct module** deps;
  const char** deps_name;
  size_t deps_cnt;
  size_t refs_cnt;
  enum module_flag flag;
  module_entry_t entry;
  module_exit_t exit;
  u32 magic;
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
 * @return struct module* Loaded module.
 */
struct module*
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
 * @return struct module* Module pointer.
 */
struct module*
find_module(const char* name);

/**
 * @brief Find loaded module dependency.
 *
 * @param mod Loaded module.
 * @param name Dependency name.
 * @return struct module* Dependency.
 */
struct module*
find_module_dep(struct module* mod, const char* name);

/**
 * @brief Default exit function for modules that cannot be unloaded.
 *
 * @param mod Loaded module.
 */
void
__module_default_exit(struct module* mod);
