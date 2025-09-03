/**
 * @file sync.h
 * @author Dessera (dessera@qq.com)
 * @brief Task sync utils.
 * @version 0.1.0
 * @date 2025-08-24
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/core/intr.h"
#include "ordos/lib/common.h" // IWYU pragma: keep
#include "ordos/lib/types.h"
#include "ordos/lib/util/list_head.h"

/**
 * @brief COW spin lock.
 *
 */
struct spin_lock
{
  volatile i32 flag;
};

/**
 * @brief Init spin lock.
 *
 * @param lock Spin lock.
 */
void
spin_lock_init(struct spin_lock* lock);

/**
 * @brief Lock spin lock.
 *
 * @param lock Spin lock.
 */
void
spin_lock(struct spin_lock* lock);

/**
 * @brief Unlock spin lock.
 *
 * @param lock Spin lock.
 */
void
spin_unlock(struct spin_lock* lock);

/**
 * @brief Lock interrupt.
 *
 * @return bool Old interrupt status.
 */
__inline static bool
intr_lock(void)
{
  return intr_set_status(false);
}

/**
 * @brief Unlock interrupt.
 *
 * @param flag Old interrupt status.
 */
__inline static void
intr_unlock(bool flag)
{
  intr_set_status(flag);
}

/**
 * @brief Mutex lock.
 *
 */
struct mutex_lock
{
  volatile u8 flag;
  struct spin_lock guard;
  struct list_head wait_queue;
};

/**
 * @brief Init mutex lock.
 *
 * @param lck MUtex lock.
 */
void
mutex_lock_init(struct mutex_lock* lck);

/**
 * @brief Acquire mutex.
 *
 * @param lck Lock.
 */
void
mutex_lock(struct mutex_lock* lck);

/**
 * @brief Unlock mutex.
 *
 * @param lck Lock.
 */
void
mutex_unlock(struct mutex_lock* lck);

/**
 * @brief Semaphore.
 *
 */
struct semaphore
{
  volatile size_t value;
  struct spin_lock guard;
  struct list_head wait_queue;
};

/**
 * @brief Init semaphore.
 *
 * @param sem Semaphore.
 * @param value Init value.
 */
void
semaphore_init(struct semaphore* sem, size_t value);

/**
 * @brief Acquire semaphore.
 *
 * @param sem Semaphore.
 */
void
semaphore_down(struct semaphore* sem);

/**
 * @brief Acquire semaphore (in no interrupt context).
 *
 * @param sem Semaphore.
 */
void
semaphore_down_intr(struct semaphore* sem);

/**
 * @brief Release semaphore.
 *
 * @param sem Semaphore.
 */
void
semaphore_up(struct semaphore* sem);

/**
 * @brief Release semaphore (in no interrupt context).
 *
 * @param sem Semaphore.
 */
void
semaphore_up_intr(struct semaphore* sem);
