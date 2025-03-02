#include "kernel/utils/print.h"
#include "kernel/screen.h"
#include "kernel/utils/mm.h"

static u16
__kputchar(u8 c, u16 cursor)
{
  __asm__ volatile("movw %%ax, (%%ebx)"
                   :
                   : "b"(GET_VIDEO_ADDR(cursor)), "a"(c | (0x07 << 8)));

  return cursor + 1;
}

static u16
__kputcr(u16 cursor)
{
  cursor -= (cursor % SCREEN_WIDTH);
  return cursor;
}

static u16
__kputlf(u16 cursor)
{
  cursor += SCREEN_WIDTH - (cursor % SCREEN_WIDTH);
  return cursor;
}

void
kscrscroll(size_t rows)
{
  if (rows == 0 || rows >= SCREEN_HEIGHT) {
    return;
  }
  kmemcpy((void*)GET_VIDEO_ADDR(0),
          (void*)GET_VIDEO_ADDR(GET_CURSOR(rows, 0)),
          GET_VIDEO_SIZE(SCREEN_HEIGHT - rows));
  kmemset((void*)GET_VIDEO_ADDR(GET_CURSOR(SCREEN_HEIGHT - rows, 0)),
          0,
          GET_VIDEO_SIZE(rows));
}

size_t
kputs(const char* str)
{
  size_t len = 0;
  while (*str != '\0') {
    len += kputchar(*str++);
  }
  return len;
}

size_t
kputchar(u8 c)
{
  u16 cursor = kget_cursor();
  switch (c) {
    case '\n':
      cursor = __kputlf(cursor);
      break;
    case '\r':
      cursor = __kputcr(cursor);
      break;
    default:
      cursor = __kputchar(c, cursor);
  }

  // check if cursor is at the end of the screen
  if (cursor >= SCREEN_SIZE) {
    // scroll screen
    kscrscroll(1);
    // set cursor to the beginning of the last line
    cursor = GET_CURSOR(SCREEN_HEIGHT - 1, 0);
  }

  kset_cursor(cursor);

  return 1;
}

void
kscrclear(void)
{
  for (size_t i = 0; i < SCREEN_SIZE; i++) {
    __asm__ volatile("movw $0x0720, (%%ebx)" : : "b"(GET_VIDEO_ADDR(i)));
  }

  kset_cursor(0);
}

u16
kget_cursor(void)
{
  u8 cursor_hsb = 0;
  u8 cursor_lsb = 0;
  __asm__ volatile("outb %%al, %%dx"
                   :
                   : "d"(SCREEN_CMD), "a"(SCREEN_CMD_CURSOR_HIGH));
  __asm__ volatile("inb %%dx, %%al;" : "=a"(cursor_hsb) : "d"(SCREEN_DATA));

  __asm__ volatile("outb %%al, %%dx"
                   :
                   : "d"(SCREEN_CMD), "a"(SCREEN_CMD_CURSOR_LOW));
  __asm__ volatile("inb %%dx, %%al;" : "=a"(cursor_lsb) : "d"(SCREEN_DATA));

  return ((u16)cursor_hsb << 8) | (u16)cursor_lsb;
}

void
kset_cursor(u16 cur)
{
  u8 cursor_hsb = (cur >> 8);
  u8 cursor_lsb = (cur & 0xff);
  __asm__ volatile("outb %%al, %%dx"
                   :
                   : "d"(SCREEN_CMD), "a"(SCREEN_CMD_CURSOR_HIGH));
  __asm__ volatile("outb %%al, %%dx" : : "a"(cursor_hsb), "d"(SCREEN_DATA));

  __asm__ volatile("outb %%al, %%dx"
                   :
                   : "d"(SCREEN_CMD), "a"(SCREEN_CMD_CURSOR_LOW));
  __asm__ volatile("outb %%al, %%dx" : : "a"(cursor_lsb), "d"(SCREEN_DATA));
}