/**
 * @file pit.h
 * @author Dessera (dessera@qq.com)
 * @brief PIT driver.
 * @version 0.1.0
 * @date 2025-08-26
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/lib/common.h" // IWYU pragma: keep
#include "ordos/lib/types.h"

#define PIT_INPUT_FREQ 1193180 /**< PIT input freq. */

/**
 * @brief PIT ticks handler.
 *
 */
typedef void (*pit_handler_t)(size_t);

/**
 * @brief Get current system frequency.
 *
 * @return u16 Frequency.
 */
u16
get_sys_freq(void);

/**
 * @brief Set current system frenquency.
 *
 * @param freq New Frequency.
 */
void
set_sys_freq(u16 freq);

/**
 * @brief Get system ticks.
 *
 * @return size_t Ticks.
 */
size_t
get_ticks(void);

/**
 * @brief Register a tick handler.
 *
 * @param ticks Ticks.
 * @param handler Tick handler.
 * @return int Result.
 */
int
pit_register(size_t ticks, pit_handler_t handler);
