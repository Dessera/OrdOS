#pragma once

#include "kernel/stddef.h"

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

ssize_t
kmemcmp(const void* s1, const void* s2, size_t n);