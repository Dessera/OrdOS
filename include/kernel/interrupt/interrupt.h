#pragma once

#include "kernel/config/interrupt.h"
#include "lib/types.h"

/**
 * @brief Interrupt handler function pointer type
 */
typedef void (*interrupt_handler_t)(u32);

enum intr_type
{
  INTR_TYPE_PAGE_FAULT = 0x0e,
  INTR_TYPE_TIMER = 0x20,
  INTR_TYPE_KEYBOARD = 0x21,
  INTR_TYPE_IDE0 = 0x2E,
  INTR_TYPE_IDE1 = 0x2F,
  INTR_TYPE_SYSCALL = 0x80,
};

/**
 * @brief Initializes the interrupt subsystem
 */
void
init_intr(void);

/**
 * @brief Registers a handler for a specific interrupt
 *
 * @param interrupt_number The interrupt number to register the handler for
 * @param handler The handler to register
 */
void
intr_register_handler(u32 interrupt_number, interrupt_handler_t handler);

/**
 * @brief Gets the current interrupt status
 *
 * @return true If interrupts are enabled
 * @return false If interrupts are disabled
 */
bool
intr_get_status(void);

/**
 * @brief Sets the current interrupt status
 *
 * @param status The new interrupt status
 * @return true If interrupts were enabled before setting
 * @return false If interrupts were disabled before setting
 */
bool
intr_set_status(bool status);
