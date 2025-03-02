#pragma once

#define SCREEN_WIDTH 0x50
#define SCREEN_HEIGHT 0x19
#define SCREEN_BUFFER_ADDR 0xb8000

#define SCREEN_CMD 0x3d4
#define SCREEN_DATA 0x3d5

#define SCREEN_CMD_CURSOR_HIGH 0x0e
#define SCREEN_CMD_CURSOR_LOW 0x0f

#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)
#define SCREEN_BUFFER_SIZE (SCREEN_SIZE * 2)

#define GET_VIDEO_ADDR(cursor) (SCREEN_BUFFER_ADDR + ((cursor) * 2))
#define GET_CURSOR(row, col) ((row) * SCREEN_WIDTH + (col))

#define GET_VIDEO_SIZE(rows) ((rows) * SCREEN_WIDTH * 2)
