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
#include "ordos/lib/common.h"
#include "ordos/lib/types.h"

#define VGA_BUF_WIDTH 0x50
#define VGA_BUF_HEIGHT 0x19
#define VGA_BUF_ADDR (0xB8000 + ORDOS_KERNEL_VADDR)

#define VGA_DP_SIZE (VGA_BUF_WIDTH * VGA_BUF_HEIGHT)
#define VGA_BUF_SIZE (VGA_DP_SIZE * 2)

#define VGA_PORT_CMD 0x3d4
#define VGA_PORT_DATA 0x3d5

#define VGA_CMD_CURSOR_HIGH 0x0e
#define VGA_CMD_CURSOR_LOW 0x0f

#define VGA_BLANK 0x0720

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
 * @brief Create VGA cursor.
 *
 * @param row Cursor row.
 * @param col Cursor col.
 * @return vga_cursor_t VGA cursor.
 */
static ORDOS_FORCEINLINE vga_cursor_t
vga_create_cursor(u16 row, u16 col)
{
  return (row * VGA_BUF_WIDTH) + col;
}
