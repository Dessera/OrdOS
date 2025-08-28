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
#include "ordos/lib/expr.h"
#include "ordos/lib/section.h"
#include "ordos/lib/types.h"

#define __module_concat_impl(lhs, rhs) lhs##rhs
#define __module_concat(lhs, rhs) __module_concat_impl(lhs, rhs)

/**
 * @brief Get module real name.
 *
 */
#define __module_name(name)                                                    \
  __module_concat(__module_concat(__module_concat(__, name), _mod_),           \
                  ORDOS_MODULE_MAGIC)

/**
 * @brief Get module deps array name.
 *
 */
#define __module_deps_name(name) __module_concat(__module_name(name), _deps)

/**
 * @brief Get dep pointer.
 *
 */
#define __module_ptr(name) &__module_name(name)

/**
 * @brief Convert deps to array.
 *
 */
#define __module_deps(...)                                                     \
  { __recursive_apply_with_comma(__module_ptr, __VA_ARGS__) }

/**
 * @brief Util to declare module dependency.
 *
 */
#define module_dependency(name) extern struct module __module_name(name)

/**
 * @brief Util to define a kernel module.
 *
 */
#define module_init(mname, mflag, mentry, mexit, ...)                          \
  __used static struct module* __module_deps_name(name)[] =                    \
    __module_deps(__VA_ARGS__);                                                \
  __module struct module __module_name(                                        \
    mname) = { .name = #mname,                                                 \
               .deps = __module_deps_name(name),                               \
               .deps_cnt = countof(__module_deps_name(name)),                  \
               .refs_cnt = 0,                                                  \
               .flag = mflag,                                                  \
               .entry = mentry,                                                \
               .exit = mexit,                                                  \
               .magic = ORDOS_MODULE_MAGIC }

/**
 * @brief Util to define a noexit kernel module.
 *
 */
#define module_init_noexit(mname, mflag, mentry, ...)                          \
  module_init(mname, mflag, mentry, __module_default_exit, ##__VA_ARGS__)

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

/**
 * @brief Module flags.
 *
 */
enum module_flag
{
  MOD_NOLOAD = 0x00,   // Module will not be loaded automatically.
  MOD_AUTOLOAD = 0x01, // Module will be loaded automatically.
  MOD_SYSMOD = 0x00,   // Module is system module (common module).
  MOD_COREMOD = 0x02,  // Module is core module (kernel basic subsystem).
};

/**
 * @brief Kernel module type.
 *
 */
struct module
{
  const char* name;
  struct module** deps;
  size_t deps_cnt;
  size_t refs_cnt;
  int flag;
  module_entry_t entry;
  module_exit_t exit;
  u32 magic;
};

/**
 * @brief Initialize module subsystem.
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
 * @return struct module* Module pointer.
 */
struct module*
find_module(const char* name);

/**
 * @brief Load all module which has specific flags (and MOD_AUTOLOAD).
 *
 * @param flags Module flags, will be ignored.
 * @return size_t Loaded module (no deps).
 * @note Do not increase `refs_cnt` if module has loaded.
 */
size_t
autoload_module(int flags);

/**
 * @brief Default exit function for modules that cannot be unloaded.
 *
 * @param mod Loaded module.
 */
void
__module_default_exit(struct module* mod);
