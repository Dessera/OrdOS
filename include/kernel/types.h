#pragma once

#define OFFSET_OF(type, member) __builtin_offsetof(type, member)

#define CONTAINER_OF(ptr, type, member)                                        \
  ({                                                                           \
    const typeof(((type*)0)->member)* __mptr = (ptr);                          \
    (type*)((char*)__mptr - OFFSET_OF(type, member));                          \
  })

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef true
#define true (1)
#endif

#ifndef false
#define false (0)
#endif

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

#define MAX_SIZE_T ((size_t)-1)
#define MIN_SIZE_T ((size_t)0)
#define MAX_SSIZE_T ((ssize_t)0x7FFFFFFF)
#define MIN_SSIZE_T ((ssize_t)0x80000000)

typedef _Bool bool;
