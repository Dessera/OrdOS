#include "kernel/utils/string.h"
#include "kernel/assert.h"
#include "kernel/types.h"

char*
kstrcpy(char* dest, const char* src)
{
  KASSERT(dest != NULL && src != NULL);
  char* dest_ptr = dest;
  while ((*dest++ = *src++))
    ;
  return dest_ptr;
}

size_t
kstrlen(const char* str)
{
  KASSERT(str != NULL);
  const char* str_ptr = str;
  while (*str_ptr) {
    str_ptr++;
  }
  return str_ptr - str;
}

ssize_t
kstrcmp(const char* str1, const char* str2)
{
  KASSERT(str1 != NULL && str2 != NULL);
  while (*str1 && (*str1 == *str2)) {
    str1++;
    str2++;
  }
  return *str2 < *str1 ? -1 : *str1 == *str2 ? 0 : 1;
}

char*
kstrchr(const char* str, const char c)
{
  KASSERT(str != NULL);
  while (*str && *str != c) {
    str++;
  }
  return *str == c ? (char*)str : NULL;
}

char*
kstrrchr(const char* str, const char c)
{
  KASSERT(str != NULL);
  const char* last_ptr = NULL;
  while (*str) {
    if (*str == c) {
      last_ptr = str;
    }
    str++;
  }
  return (char*)last_ptr;
}

char*
kstrcat(char* dest, const char* src)
{
  KASSERT(dest != NULL && src != NULL);
  char* dest_ptr = dest;
  while (*dest) {
    dest++;
  }
  kstrcpy(dest, src);
  return dest_ptr;
}

size_t
kstrchrs(const char* str, const char c)
{
  KASSERT(str != NULL);
  size_t count = 0;
  while (*str) {
    if (*str == c) {
      count++;
    }
    str++;
  }
  return count;
}

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