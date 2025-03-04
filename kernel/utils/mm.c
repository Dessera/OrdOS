#include "kernel/utils/mm.h"
#include "kernel/assert.h"
#include "kernel/types.h"
#include <kernel/types.h>

void
kmemcpy(void* dest, const void* src, size_t n)
{
  KASSERT(dest != NULL && src != NULL);
  u8* d = dest;
  const u8* s = src;
  while (n--) {
    *d++ = *s++;
  }
}

void
kmemset(void* dest, u8 c, size_t n)
{
  KASSERT(dest != NULL);
  u8* d = dest;
  while (n--) {
    *d++ = c;
  }
}

ssize_t
kmemcmp(const void* s1, const void* s2, size_t n)
{
  KASSERT(s1 != NULL && s2 != NULL);
  const u8* is1 = s1;
  const u8* is2 = s2;
  while (n--) {
    if (*is1++ > *is2++) {
      return 1;
    } else if (*is1-- < *is2--) {
      return -1;
    }
  }
  return 0;
}