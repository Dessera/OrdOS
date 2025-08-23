/**
 * @file vga.h
 * @author Dessera (dessera@qq.com)
 * @brief VGA buffer driver.
 * @version 0.1.0
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/kernel/config.h"
#include "ordos/lib/common.h" // IWYU pragma: keep
#include "ordos/lib/types.h"

#define VGA_BUF_WIDTH 0x50  /**< VGA buffer width. */
#define VGA_BUF_HEIGHT 0x19 /**< VGA buffer height. */

#define VGA_BUF_ADDR                                                           \
  (0xB8000 + ORDOS_KERNEL_VADDR) /**<  VGA buffer address (vaddr). */

#define VGA_DP_SIZE                                                            \
  (VGA_BUF_WIDTH * VGA_BUF_HEIGHT)     /**< VGA buffer size (in WORD). */
#define VGA_BUF_SIZE (VGA_DP_SIZE * 2) /**< VGA buffer size (in BYTE). */

#define VGA_PORT_CMD 0x3D4  /**< VGA io port used by command. */
#define VGA_PORT_DATA 0x3D5 /**< VGA io port used by data. */

#define VGA_CMD_CURSOR_HIGH 0x0E /**< VGA set or get high cursor command. */
#define VGA_CMD_CURSOR_LOW 0x0F  /**< VGA set or get low cursor command. */

#define VGA_BLANK 0x0720 /**< VGA blank character. */

/**
 * @brief VGA curosr.
 *
 */
typedef u16 vga_cursor_t;

/**
 * @brief Set value in VGA buffer.
 *
 * @param cursor VGA cursor.
 * @param value Buffer value.
 */
void
vga_set(vga_cursor_t cursor, u16 value);

/**
 * @brief Get value in VGA buffer.
 *
 * @param cursor VGA cursor.
 * @return u16 Buffer value.
 */
u16
vga_get(vga_cursor_t cursor);

/**
 * @brief Set VGA cursor.
 *
 * @param cursor VGA cursor.
 */
void
vga_set_cursor(vga_cursor_t cursor);

/**
 * @brief Get VGA cursor.
 *
 * @return vga_cursor_t VGA cursor.
 */
vga_cursor_t
vga_get_cursor(void);

/**
 * @brief Clear VGA buffer.
 *
 */
void
vga_clear(void);

/**
 * @brief Scroll VGA buffer.
 *
 * @param offs Line offset.
 */
void
vga_scroll(size_t offs);

/**
 * @brief Util to create VGA character.
 *
 * @param ch Character.
 * @return u16 VGA display character.
 */
__inline static u16
vga_create_char(char ch)
{
  // TODO: Color
  return (u16)ch | (0x07 << 8); // NOLINT
}

/**
 * @brief Util to create VGA cursor.
 *
 * @param row Cursor row.
 * @param col Cursor column.
 * @return vga_cursor_t Cursor.
 */
__inline vga_cursor_t
vga_create_cursor(u16 row, u16 col)
{
  return (row * VGA_BUF_WIDTH) + col;
}
