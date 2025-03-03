#include "kernel/utils/string.h"
#include "kernel/assert.h"

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

i32
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