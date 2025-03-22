#include "user/string.h"
#include "lib/types.h"

char*
strcpy(char* dest, const char* src)
{
  char* dest_ptr = dest;
  while ((*dest++ = *src++))
    ;
  return dest_ptr;
}

size_t
strlen(const char* str)
{
  const char* str_ptr = str;
  while (*str_ptr) {
    str_ptr++;
  }
  return str_ptr - str;
}

ssize_t
strcmp(const char* str1, const char* str2)
{
  while (*str1 && (*str1 == *str2)) {
    str1++;
    str2++;
  }
  return *str2 < *str1 ? -1 : *str1 == *str2 ? 0 : 1;
}

char*
strchr(const char* str, const char c)
{
  while (*str && *str != c) {
    str++;
  }
  return *str == c ? (char*)str : NULL;
}

char*
strrchr(const char* str, const char c)
{
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
strcat(char* dest, const char* src)
{
  char* dest_ptr = dest;
  while (*dest) {
    dest++;
  }
  strcpy(dest, src);
  return dest_ptr;
}

size_t
strchrs(const char* str, const char c)
{
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
memcpy(void* dest, const void* src, size_t n)
{
  u8* d = dest;
  const u8* s = src;
  while (n--) {
    *d++ = *s++;
  }
}

void
memset(void* dest, u8 c, size_t n)
{
  u8* d = dest;
  while (n--) {
    *d++ = c;
  }
}

ssize_t
memcmp(const void* s1, const void* s2, size_t n)
{
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