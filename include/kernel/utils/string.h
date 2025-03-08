#pragma once

#include "kernel/types.h"

char*
kstrcpy(char* dest, const char* src);

size_t
kstrlen(const char* str);

ssize_t
kstrcmp(const char* str1, const char* str2);

char*
kstrchr(const char* str, const char c);

char*
kstrrchr(const char* str, const char c);

char*
kstrcat(char* dest, const char* src);

size_t
kstrchrs(const char* str, const char c);

/**
 * @brief Copy memory from one location to another
 *
 * @param dest destination
 * @param src source
 * @param n number of bytes to copy
 */
void
kmemcpy(void* dest, const void* src, size_t n);

/**
 * @brief Set memory to a value
 *
 * @param dest destination
 * @param c value to set
 * @param n number of bytes to set
 */
void
kmemset(void* dest, u8 c, size_t n);

/**
 * @brief Compare memory
 *
 * @param s1 first memory location
 * @param s2 second memory location
 * @param n number of bytes to compare
 * @return ssize_t return 0 if equal, -1 if s1 < s2, 1 if s1 > s2
 */
ssize_t
kmemcmp(const void* s1, const void* s2, size_t n);