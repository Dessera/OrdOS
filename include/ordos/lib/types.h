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
