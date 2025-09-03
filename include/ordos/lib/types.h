/**
 * @file types.h
 * @author Dessera (dessera@qq.com)
 * @brief Type utils.
 * @version 0.1.0
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/lib/common.h" // IWYU pragma: keep

#define NULL ((void*)0)

#define NPOS (-1)

#define BIT (1)
#define BYTE (sizeof(u8) * 8)
#define WORD (sizeof(u16) * 8)
#define DWORD (sizeof(u32) * 8)
#define QWORD (sizeof(u64) * 8)

#define KBYTES (1024)
#define MBYTES (KBYTES * 1024)

#define BITMASK 1
#define BYTEMASK (0xFF)
#define WORDMASK (0xFFFF)
#define DWORDMASK (0xFFFFFFFF)
#define QWORDMASK (0xFFFFFFFFFFFFFFFF)

#define U8_MAX ((u8) - 1)
#define U8_MIN ((u8)0)
#define USIZE_MAX ((size_t)-1)
#define USIZE_MIN ((size_t)0)
#define ISIZE_MAX ((ssize_t)0x7FFFFFFF)
#define ISIZE_MIN ((ssize_t)0)

#define offset_of(type, member) __builtin_offsetof(type, member)

#define container_of(ptr, type, member)                                        \
  ({                                                                           \
    typeof(((type*)0)->member)* mptr = (ptr);                                  \
    (type*)((char*)mptr - offset_of(type, member));                            \
  })

#define countof(arr) (sizeof(arr) / sizeof(arr[0]))

#define unused(var) ((void)var)

#define has_flags(flags, req) (((flags) & (req)) == (req))
#define mask_flags(flags, req) ((flags) & (~(req)))

#define va_list __builtin_va_list
#define va_satrt __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef u32 size_t;
typedef i32 ssize_t;

typedef u32 uintptr_t;
typedef i32 intptr_t;

typedef _Bool bool;

/**
 * @brief Boolean literal.
 *
 */
enum
{
  true = 1,
  false = 0
};

/**
 * @brief Number base flags.
 *
 */
enum base_flag
{
  BASE_BIN = 2,
  BASE_OCT = 8,
  BASE_DEC = 10,
  BASE_HEX = 16
};

/**
 * @brief Convert int to string.
 *
 * @param buffer Destination buffer.
 * @param value Number to be converted.
 * @param base Convertion base.
 */
void
itoa(char* buffer, i32 value, u8 base);

/**
 * @brief Convert unsigned int to string.
 *
 * @param buffer Destination buffer.
 * @param value Number to be converted.
 * @param base Convertion base.
 */
void
utoa(char* buffer, u32 value, u8 base);

/**
 * @brief Check if character is digit.
 *
 * @param c Character.
 * @return bool true if character is digit, otherwise false.
 */
__inline static bool
is_digit(char c)
{
  return c >= '0' && c <= '9';
}

/**
 * @brief Convert character to digit.
 *
 * @param c Character.
 * @return int Converted digit.
 */
__inline static int
to_digit(char c)
{
  return c - '0';
}
