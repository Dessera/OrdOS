/**
 * @file kthread.h
 * @author Dessera (dessera@qq.com)
 * @brief Kthread utils.
 * @version 0.1.0
 * @date 2025-08-29
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/core/task.h"

extern struct task* __kmain;

extern struct task* __kidle;

/**
 * @brief Init kmain.
 *
 * @return int Init status.
 */
int
init_kmain(void);

/**
 * @brief Init kidle.
 *
 * @return int Init status.
 */
int
init_kidle(void);

/**
 * @brief Create kernel thread.
 *
 * @param name Task name.
 * @param prio Task priority.
 * @param entry Task entry.
 * @param arg Task argument.
 * @return struct task* Created task.
 */
struct task*
kthread_create(const char* name,
               task_prio_t prio,
               task_entry_t entry,
               void* arg);
