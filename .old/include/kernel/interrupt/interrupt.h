#pragma once

#include "lib/types.h"

/**
 * @brief Interrupt handler function pointer type
 */
typedef void (*interrupt_handler_t)(u32);

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

/**
 * @brief Intrerrupt entry point
 *
 * @param irq The interrupt number
 */
void
intr_common_handler(u32 irq);
