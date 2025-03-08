#include "kernel/utils/print.h"
#include "kernel/config/utils.h"
#include "kernel/device/vga.h"
#include "kernel/types.h"
#include "kernel/utils/asm.h"
#include "kernel/utils/string.h"

static u16
__kputchar(u8 c, u16 cursor)
{
  u16 payload = (u16)c | (0x07 << 8);
  u16* addr = (u16*)VGA_GET_ADDR(cursor);
  *addr = payload;

  return cursor + 1;
}

static u16
__kputcr(u16 cursor)
{
  cursor -= (cursor % VGA_BUF_WIDTH);
  return cursor;
}

static u16
__kputlf(u16 cursor)
{
  cursor += VGA_BUF_WIDTH - (cursor % VGA_BUF_WIDTH);
  return cursor;
}

static u16
__kputbs(u16 cursor)
{
  return cursor - 1;
}

void
kscrscroll(size_t rows)
{
  if (rows == 0) {
    return;
  } else if (rows >= VGA_BUF_HEIGHT) {
    kscrclear();
    return;
  }

  kmemcpy((void*)VGA_GET_ADDR(0),
          (void*)VGA_GET_ADDR(VGA_GET_CURSOR(rows, 0)),
          VGA_GET_BUF_SIZE(VGA_BUF_HEIGHT - rows));
  kmemset((void*)VGA_GET_ADDR(VGA_GET_CURSOR(VGA_BUF_HEIGHT - rows, 0)),
          0,
          VGA_GET_BUF_SIZE(rows));
}

void
kputs(const char* str)
{
  while (*str != '\0') {
    kputchar(*str++);
  }
}

void
kput_u32(u32 num)
{
  if (num < 10) {
    kputchar(num + '0');
  } else {
    kput_u32(num / 10);
    kputchar(num % 10 + '0');
  }
}

void
kputchar(char c)
{
  u16 cursor = kget_cursor();
  switch (c) {
    case '\n':
      cursor = __kputlf(cursor);
      break;
    case '\r':
      cursor = __kputcr(cursor);
      break;
    case '\b':
      cursor = __kputbs(cursor);
      break;
    default:
      cursor = __kputchar(c, cursor);
  }

  // check if cursor is at the end of the screen
  if (cursor >= VGA_DP_SIZE) {
    // scroll screen
    kscrscroll(1);
    // set cursor to the beginning of the last line
    cursor = VGA_GET_CURSOR(VGA_BUF_HEIGHT - 1, 0);
  }

  kset_cursor(cursor);
}

void
kscrclear(void)
{
  u16* cursor = (u16*)0;
  for (size_t i = 0; i < VGA_DP_SIZE; i++) {
    cursor = (u16*)VGA_GET_ADDR(i);
    *cursor = 0x0720;
  }

  kset_cursor(0);
}

u16
kget_cursor(void)
{
  outb(VGA_PORT_CMD, VGA_CMD_CURSOR_HIGH);
  u8 cursor_hb = inb(VGA_PORT_DATA);

  outb(VGA_PORT_CMD, VGA_CMD_CURSOR_LOW);
  u8 cursor_lb = inb(VGA_PORT_DATA);

  return ((u16)cursor_hb << 8) | (u16)cursor_lb;
}

void
kset_cursor(u16 cur)
{
  if (cur >= VGA_DP_SIZE) {
    return;
  }
  u8 cursor_hb = (cur >> 8);
  u8 cursor_lb = (cur & 0xff);
  outb(VGA_PORT_CMD, VGA_CMD_CURSOR_HIGH);
  outb(VGA_PORT_DATA, cursor_hb);
  outb(VGA_PORT_CMD, VGA_CMD_CURSOR_LOW);
  outb(VGA_PORT_DATA, cursor_lb);
}

static void
__kvsprint_i32(char** pbuf, const char** pfmt, i32 num, u8 base)
{
  itoa(*pbuf, num, base);
  *pbuf += kstrlen(*pbuf);
  *pfmt += 2;
}

static void
__kvsprint_u32(char** pbuf, const char** pfmt, u32 num, u8 base)
{
  utoa(*pbuf, num, base);
  *pbuf += kstrlen(*pbuf);
  *pfmt += 2;
}

static void
__kvsprint_s(char** pbuf, const char** pfmt, const char* str)
{
  size_t len = kstrlen(str);
  kmemcpy(*pbuf, str, len);
  *pbuf += len;
  *pfmt += 2;
}

static void
__kvsprint_percent(char** pbuf, const char** pfmt)
{
  **pbuf = '%';
  (*pbuf)++;
  *pfmt += 2;
}

static void
__kvsprint_default(char** pbuf, const char** pfmt)
{
  **pbuf = **pfmt;
  (*pbuf)++;
  (*pfmt)++;
}

void
kprint(const char* str, ...)
{
  char buf[UTILS_PRINT_BUFSIZE];
  VA_LIST args;
  VA_START(args, str);
  kvsprint(buf, str, args);
  VA_END(args);

  kputs(buf);
}

void
kprintln(const char* str, ...)
{
  char buf[UTILS_PRINT_BUFSIZE];
  VA_LIST args;
  VA_START(args, str);
  kvsprint(buf, str, args);
  VA_END(args);

  kputs(buf);
  kputs("\n");
}

void
ksprint(char* buf, const char* str, ...)
{
  VA_LIST args;
  VA_START(args, str);
  kvsprint(buf, str, args);
  VA_END(args);
}

void
kvsprint(char* buf, const char* fmt, VA_LIST args)
{
  while (*fmt != '\0') {
    if (*fmt == '%') {
      switch (*(fmt + 1)) {
        case 'i':
        case 'd':
          __kvsprint_i32(&buf, &fmt, VA_ARG(args, i32), 10);
          break;
        case 'u':
          __kvsprint_u32(&buf, &fmt, VA_ARG(args, u32), 10);
          break;
        case 'X':
        case 'x':
          __kvsprint_u32(&buf, &fmt, VA_ARG(args, u32), 16);
          break;
        case 's':
          __kvsprint_s(&buf, &fmt, VA_ARG(args, const char*));
          break;
        case '%':
          __kvsprint_percent(&buf, &fmt);
          break;
        default:
          __kvsprint_default(&buf, &fmt);
          break;
      }
    } else {
      __kvsprint_default(&buf, &fmt);
    }
  }
}
