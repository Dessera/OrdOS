#pragma once

#define VGA_BUF_WIDTH 0x50
#define VGA_BUF_HEIGHT 0x19
#define VGA_BUF_ADDR 0xb8000

#define VGA_PORT_CMD 0x3d4
#define VGA_PORT_DATA 0x3d5

#define VGA_CMD_CURSOR_HIGH 0x0e
#define VGA_CMD_CURSOR_LOW 0x0f

#define VGA_DP_SIZE (VGA_BUF_WIDTH * VGA_BUF_HEIGHT)
#define VGA_BUF_SIZE (VGA_DP_SIZE * 2)

#define VGA_GET_ADDR(cursor) (VGA_BUF_ADDR + ((cursor) * 2))
#define VGA_GET_CURSOR(row, col) ((row) * VGA_BUF_WIDTH + (col))

#define VGA_GET_BUF_SIZE(rows) ((rows) * VGA_BUF_WIDTH * 2)
