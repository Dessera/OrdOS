

#include "lib/types.h"

static char digits[] = "0123456789abcdef";

static void
__itoa_impl(char** buffer, i32 value, u8 base)
{
  if (value < 0) {
    **buffer = '-';
    ++(*buffer);
    value = -value;
  }

  if (value < base) {
    **buffer = digits[value];
    ++(*buffer);
  } else {
    __itoa_impl(buffer, value / base, base);
    __itoa_impl(buffer, value % base, base);
  }

  **buffer = '\0';
}

void
itoa(char* buffer, i32 value, u8 base)
{
  __itoa_impl(&buffer, value, base);
}

static void
__utoa_impl(char** buffer, u32 value, u8 base)
{
  if (value < base) {
    **buffer = digits[value];
    ++(*buffer);
  } else {
    __utoa_impl(buffer, value / base, base);
    __utoa_impl(buffer, value % base, base);
  }
}

void
utoa(char* buffer, u32 value, u8 base)
{
  __utoa_impl(&buffer, value, base);
}