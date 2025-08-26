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
 * @brief PIT command port.
 *
 */
enum pit_io_port : u16
{
  PIT_CHANNEL0 = 0x40,
  PIT_CHANNEL1 = 0x41,
  PIT_CHANNEL2 = 0x42,
  PIT_MODEREG = 0x43,
};

/**
 * @brief PIT data flags.
 *
 */
enum pit_flag : u8
{
  PIT_SEL_C0 = 0x00, // Select Channel0
  PIT_SEL_C1 = 0x40, // Select Channel1
  PIT_SEL_C2 = 0x80, // Select Channel2

  PIT_LATCHCNT = 0x00,
  PIT_LBONLY = 0x10,
  PIT_HBONLY = 0x20,
  PIT_LBHB = 0x30,

  PIT_INTR = 0x00,
  PIT_ONESHOT = 0x02,
  PIT_RATE = 0x04,
  PIT_SQUARE = 0x06,
  PIT_SWSTROBE = 0x08,
  PIT_HWSTROBE = 0x0A,

  PIT_BIN = 0x00,
  PIT_BCD = 0x01,
};

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
