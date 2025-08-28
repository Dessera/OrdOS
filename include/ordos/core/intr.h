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
 * @brief Interrupt types.
 *
 */
enum intr_type : u32
{
  IE_ZERO_DIVIDE = 0x00,
  IE_DEBUG = 0x01,
  IE_NMI = 0x02,
  IE_BREAKPOINT = 0x03,
  IE_OVERFLOW = 0x04,
  IE_BOUND_RANGE = 0x05,
  IE_INVALID_OPCODE = 0x06,
  IE_DEVICE_NOT_AVAILABLE = 0x07,
  IE_DOUBLE_FAULT = 0x08,
  IE_COSEGMENT_OVERLAP = 0x09,
  IE_INVALID_TSS = 0x0A,
  IE_SEGMENT_NOT_PRESENT = 0x0B,
  IE_STACK_FAULT = 0x0C,
  IE_GENERAL_PROTECTION = 0x0D,
  IE_PAGE_FAULT = 0x0E,
  IE_RESERVED = 0x0F,
  IE_X87_FLOATING_POINT_ERROR = 0x10,
  IE_ALIGNMENT_CHECK = 0x11,
  IE_MACHINE_CHECK = 0x12,
  IE_SIMD_FLOATING_POINT_ERROR = 0x13,
  IDRV_PIT = 0x20,
  IDRV_KEYBOARD = 0x21,
  IDRV_IDE0 = 0x2E,
  IDRV_IDE1 = 0x2F,
  IUSR_SYSCALL = ORDOS_INTR_SYSCALL,
};

/**
 * @brief Interrupt handler function pointer type.
 */
typedef void (*intr_handler_t)(enum intr_type);

/**
 * @brief Registers a handler for a specific interrupt.
 *
 * @param code The interrupt code to register the handler for.
 * @param handler The handler to register.
 */
void
intr_register(enum intr_type code, intr_handler_t handler);

/**
 * @brief Unregiser a handler for a specific interrupt.
 *
 * @param code The interrupt code.
 */
void
intr_unregister(enum intr_type code);

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
