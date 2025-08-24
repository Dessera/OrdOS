/**
 * @file intr.h
 * @author Dessera (dessera@qq.com)
 * @brief Kernel interrupts.
 * @version 0.1.0
 * @date 2025-08-24
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/kernel/config.h"
#include "ordos/lib/types.h"

#define INTR_EFLAG 0x200 /**< Interrupt eflag mask. */

/**
 * @brief Interrupt handler function pointer type.
 */
typedef void (*intr_handler_t)(u32);

/**
 * @brief Interrupt code type.
 *
 */
typedef u32 intr_code_t;

/**
 * @brief Interrupt types.
 *
 */
enum intr_type : intr_code_t
{
  INTR_PAGE_FAULT = 0x0E,
  INTR_TIMER = 0x20,
  INTR_KEYBOARD = 0x21,
  INTR_IDE0 = 0x2E,
  INTR_IDE1 = 0x2F,
  INTR_SYSCALL = ORDOS_INTR_SYSCALL,
};

/**
 * @brief Initializes the interrupt subsystem.
 */
void
init_intr(void);

/**
 * @brief Registers a handler for a specific interrupt.
 *
 * @param code The interrupt code to register the handler for.
 * @param handler The handler to register.
 */
void
intr_register(intr_code_t code, intr_handler_t handler);

/**
 * @brief Unregiser a handler for a specific interrupt.
 *
 * @param code The interrupt code.
 */
void
intr_unregister(intr_code_t code);

/**
 * @brief Gets the current interrupt status.
 *
 * @return true If interrupts are enabled.
 * @return false If interrupts are disabled.
 */
bool
intr_get_status(void);

/**
 * @brief Sets the current interrupt status.
 *
 * @param status The new interrupt status.
 * @return true If interrupts were enabled before setting.
 * @return false If interrupts were disabled before setting.
 */
bool
intr_set_status(bool status);
