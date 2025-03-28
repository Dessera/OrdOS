#include "user/print.h"
#include "lib/string.h"
#include "lib/syscall.h"
#include "lib/types.h"
#include "user/config/utils.h"

static void
__vsprint_i32(char** pbuf, const char** pfmt, i32 num, u8 base)
{
  itoa(*pbuf, num, base);
  *pbuf += strlen(*pbuf);
  *pfmt += 2;
}

static void
__vsprint_u32(char** pbuf, const char** pfmt, u32 num, u8 base)
{
  utoa(*pbuf, num, base);
  *pbuf += strlen(*pbuf);
  *pfmt += 2;
}

static void
__vsprint_s(char** pbuf, const char** pfmt, const char* str)
{
  size_t len = strlen(str);
  memcpy(*pbuf, str, len);
  *pbuf += len;
  *pfmt += 2;
}

static void
__vsprint_percent(char** pbuf, const char** pfmt)
{
  **pbuf = '%';
  (*pbuf)++;
  *pfmt += 2;
}

static void
__vsprint_default(char** pbuf, const char** pfmt)
{
  **pbuf = **pfmt;
  (*pbuf)++;
  (*pfmt)++;
}

size_t
puts(const char* str)
{
  return write(0, str, strlen(str));
}

size_t
print(const char* fmt, ...)
{
  char buf[USER_UTILS_PRINT_BUFSIZE] = { 0 };
  VA_LIST args;
  VA_START(args, fmt);
  vsprint(buf, fmt, args);
  VA_END(args);

  return puts(buf);
}

size_t
println(const char* str, ...)
{
  char buf[USER_UTILS_PRINT_BUFSIZE] = { 0 };
  VA_LIST args;
  VA_START(args, str);
  AUTO buf_len = vsprint(buf, str, args);
  VA_END(args);

  buf[buf_len] = '\n';
  buf[buf_len + 1] = '\0';

  return puts(buf);
}

size_t
sprint(char* buf, const char* fmt, ...)
{
  VA_LIST args;
  VA_START(args, fmt);
  AUTO len = vsprint(buf, fmt, args);
  VA_END(args);
  return len;
}

size_t
vsprint(char* buf, const char* fmt, VA_LIST args)
{
  AUTO origin = buf;
  while (*fmt != '\0') {
    if (*fmt == '%') {
      switch (*(fmt + 1)) {
        case 'i':
        case 'd':
          __vsprint_i32(&buf, &fmt, VA_ARG(args, i32), 10);
          break;
        case 'u':
          __vsprint_u32(&buf, &fmt, VA_ARG(args, u32), 10);
          break;
        case 'p':
        case 'X':
        case 'x':
          __vsprint_u32(&buf, &fmt, VA_ARG(args, u32), 16);
          break;
        case 's':
          __vsprint_s(&buf, &fmt, VA_ARG(args, const char*));
          break;
        case '%':
          __vsprint_percent(&buf, &fmt);
          break;
        default:
          __vsprint_default(&buf, &fmt);
          break;
      }
    } else {
      __vsprint_default(&buf, &fmt);
    }
  }

  *buf = '\0';

  return strlen(origin);
}