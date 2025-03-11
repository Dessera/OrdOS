#pragma once

#define OFFSET_OF(type, member) __builtin_offsetof(type, member)

#define CONTAINER_OF(ptr, type, member)                                        \
  (type*)((char*)(ptr) - OFFSET_OF(type, member))

#define VA_LIST __builtin_va_list
#define VA_START __builtin_va_start
#define VA_END __builtin_va_end
#define VA_ARG __builtin_va_arg

#ifndef NULL
#define NULL ((void*)0)
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

#define NPOS ((ssize_t) - 1)

#define MAX_SIZE_T ((size_t)-1)
#define MIN_SIZE_T ((size_t)0)
#define MAX_SSIZE_T ((ssize_t)0x7FFFFFFF)
#define MIN_SSIZE_T ((ssize_t)0x80000000)

#define AUTO __auto_type

#define FORCE_INLINE __attribute__((always_inline)) inline

#define CLEAN_UP(type, cleaner) type __attribute__((cleanup(cleaner)))

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

void
itoa(char* buffer, i32 value, u8 base);

void
utoa(char* buffer, u32 value, u8 base);
