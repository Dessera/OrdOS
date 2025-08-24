/**
 * @file pic.h
 * @author Dessera (dessera@qq.com)
 * @brief PIC utils.
 * @version 0.1.0
 * @date 2025-08-24
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#define PIC_MASTER_CMD 0x20  /**< PIC master command port. */
#define PIC_MASTER_DATA 0x21 /**< PIC master data port. */
#define PIC_SLAVE_CMD 0xA0   /**< PIC slave command port. */
#define PIC_SLAVE_DATA 0xA1  /**< PIC slave data port. */

#define PIC_INIT 0x11 /**< PIC init command. */
#define PIC_EOI 0x20  /**< PIC eoi command. */

#ifndef __ASSEMBLER__

/**
 * @brief Initialize the Programmable Interrupt Controller.
 */
void
init_pic(void);

#endif
