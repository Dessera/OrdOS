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

/**
 * @brief Util to define a kernel module.
 *
 */
#define module_init(mname, mentry, mexit)                                      \
  __section(                                                                   \
    ".data.module") struct module __##mname = { .name = #mname,                \
                                                .entry = mentry,               \
                                                .exit = mexit,                 \
                                                .magic = ORDOS_MODULE_MAGIC };

/**
 * @brief Kernel module entry function type.
 *
 */
typedef void (*module_entry_t)(void);

/**
 * @brief Kernel module exit function type.
 *
 */
typedef void (*module_exit_t)(void);

/**
 * @brief Kernel module type.
 *
 */
struct module
{
  const char* name;
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
 * @brief Load module to kernel.
 *
 * @param mod Module pointer.
 */
void
load_module(struct module* mod);

/**
 * @brief Unload module to kernel.
 *
 * @param mod Module pointer.
 */
void
unload_module(struct module* mod);
