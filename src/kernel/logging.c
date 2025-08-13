#include "ordos/kernel/logging.h"
#include "ordos/kernel/config.h"
#include "ordos/kernel/driver/vga.h"
#include "ordos/lib/string.h" // IWYU pragma: keep
#include "ordos/lib/types.h"

static vga_cursor_t
__kputdefault(u8 c, vga_cursor_t cursor) // NOLINT
{
  // TODO: Color
  u16 payload = (u16)c | (0x07 << 8); // NOLINT
  vga_set(cursor, payload);

  return cursor + 1;
}

static vga_cursor_t
__kputcr(vga_cursor_t cursor)
{
  cursor -= (cursor % VGA_BUF_WIDTH);
  return cursor;
}

static vga_cursor_t
__kputlf(vga_cursor_t cursor)
{
  cursor += VGA_BUF_WIDTH - (cursor % VGA_BUF_WIDTH);
  return cursor;
}

static vga_cursor_t
__kputbs(vga_cursor_t cursor)
{
  return cursor - 1;
}

static void
__kputchar(char c)
{
  vga_cursor_t cursor = vga_get_cursor();
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
      cursor = __kputdefault(c, cursor);
  }

  if (cursor >= VGA_DP_SIZE) {
    vga_scroll(1);
    cursor = vga_create_cursor(VGA_BUF_HEIGHT - 1, 0);
  }

  vga_set_cursor(cursor);
}

void
init_print(void)
{
  // mutex_lock_init(&__plock);

  vga_clear();
}

void
kputs(const char* str)
{
  // mutex_lock(&__plock);

  kputs_unsafe(str);

  // mutex_unlock(&__plock);
}

void
kputs_unsafe(const char* str)
{
  while (*str != '\0') {
    __kputchar(*str++);
  }
}

static void
__kvsprint_i32(char** pbuf, const char** pfmt, i32 num, u8 base)
{
  itoa(*pbuf, num, base);
  *pbuf += strlen(*pbuf);
  *pfmt += 2;
}

static void
__kvsprint_u32(char** pbuf, const char** pfmt, u32 num, u8 base)
{
  utoa(*pbuf, num, base);
  *pbuf += strlen(*pbuf);
  *pfmt += 2;
}

static void
__kvsprint_char(char** pbuf, const char** pfmt, int c)
{
  **pbuf = (char)c;
  (*pbuf)++;
  *pfmt += 2;
}

static void
__kvsprint_s(char** pbuf, const char** pfmt, const char* str)
{
  size_t len = strlen(str);
  memcpy(*pbuf, str, len);
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
  char buf[ORDOS_IO_BUFSIZE] = { 0 };
  va_list args = NULL;
  va_satrt(args, str);
  kvsprint(buf, str, args);
  va_end(args);

  kputs(buf);
}

void
kprint_unsafe(const char* str, ...)
{
  char buf[ORDOS_IO_BUFSIZE] = { 0 };
  va_list args = NULL;
  va_satrt(args, str);
  kvsprint(buf, str, args);
  va_end(args);

  kputs_unsafe(buf);
}

void
kprintln(const char* str, ...)
{
  char buf[ORDOS_IO_BUFSIZE] = { 0 };
  va_list args = NULL;
  va_satrt(args, str);
  kvsprint(buf, str, args);
  va_end(args);

  // for thread safe, edit local buffer rather than global buffer
  size_t buf_len = strlen(buf);
  buf[buf_len] = '\n';
  buf[buf_len + 1] = '\0';

  kputs(buf);
}

void
kprintln_unsafe(const char* str, ...)
{
  char buf[ORDOS_IO_BUFSIZE] = { 0 };
  va_list args = NULL;
  va_satrt(args, str);
  kvsprint(buf, str, args);
  va_end(args);

  // for thread safe, edit local buffer rather than global buffer
  size_t buf_len = strlen(buf);
  buf[buf_len] = '\n';
  buf[buf_len + 1] = '\0';

  kputs_unsafe(buf);
}

void
ksprint(char* buf, const char* str, ...)
{
  va_list args = NULL;
  va_satrt(args, str);
  kvsprint(buf, str, args);
  va_end(args);
}

void
kvsprint(char* buf, const char* fmt, va_list args) // NOLINT
{
  while (*fmt != '\0') {
    if (*fmt == '%') {
      switch (*(fmt + 1)) {
        case 'i':
        case 'd':
          __kvsprint_i32(&buf, &fmt, va_arg(args, i32), BASE_DEC);
          break;
        case 'u':
          __kvsprint_u32(&buf, &fmt, va_arg(args, u32), BASE_DEC);
          break;
        case 'p':
        case 'X':
        case 'x':
          __kvsprint_u32(&buf, &fmt, va_arg(args, u32), BASE_HEX);
          break;
        case 's':
          __kvsprint_s(&buf, &fmt, va_arg(args, const char*));
          break;
        case 'c':
          __kvsprint_char(&buf, &fmt, va_arg(args, int));
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

  *buf = '\0';
}
