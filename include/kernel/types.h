#pragma once

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef u32 size_t;

typedef _Bool bool;

#ifndef true
#define true (1)
#endif

#ifndef false
#define false (0)
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif