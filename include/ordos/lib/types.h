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

#define NULL ((void*)0)

#define NPOS (-1)

#define BIT 1
#define BYTE (sizeof(u8) * 8)
#define WORD (sizeof(u16) * 8)
#define DWORD (sizeof(u32) * 8)
#define QWORD (sizeof(u64) * 8)

#define BITMASK 1
#define BYTEMASK (0xFF)
#define WORDMASK (0xFFFF)
#define DWORDMASK (0xFFFFFFFF)
#define QWORDMASK (0xFFFFFFFFFFFFFFFF)

#define offset_of(type, member) __builtin_offsetof(type, member)

#define container_of(ptr, type, member)                                        \
  ({                                                                           \
    typeof(((type*)0)->member)* mptr = (ptr);                                  \
    (type*)((char*)mptr - offset_of(type, member));                            \
  })

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

enum
{
  true = 1,
  false = 0
};

enum base_flag
{
  BASE_BIN = 2,
  BASE_OCT = 8,
  BASE_DEC = 10,
  BASE_HEX = 16
};

void
itoa(char* buffer, i32 value, u8 base);

void
utoa(char* buffer, u32 value, u8 base);
