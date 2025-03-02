#include "kernel/utils/mm.h"
#include "kernel/types.h"

void
kmemcpy(void* dest, const void* src, size_t n)
{
  char* d = dest;
  const char* s = src;
  while (n--) {
    *d++ = *s++;
  }
}

void
kmemset(void* dest, u8 c, size_t n)
{
  char* d = dest;
  while (n--) {
    *d++ = c;
  }
}