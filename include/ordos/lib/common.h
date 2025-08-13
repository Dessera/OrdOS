/**
 * @file common.h
 * @author Dessera (dessera@qq.com)
 * @brief OrdOS kernel common utils.
 * @version 0.1.0
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/lib/types.h" // IWYU pragma: keep

/**
 * @brief Set label to section.
 *
 */
#define ORDOS_SECTION(name) __attribute__((section(name)))

#define ORDOS_USED __attribute__((used))

/**
 * @brief Tell compiler the function is used by asm code.
 *
 */
#define ORDOS_ASMLINKAGE __attribute__((regparm(0))) ORDOS_USED

/**
 * @brief Tell compiler the function should not return.
 *
 */
#define ORDOS_NORETURN __attribute__((noreturn))

/**
 * @brief Tell compiler the function is pure.
 *
 */
#define ORDOS_PUREFN __attribute__((pure))

#define ORDOS_FORCEINLINE inline __attribute__((always_inline))

/**
 * @brief Tell compiler check function format args.
 *
 */
#define ORDOS_FORMAT(func, fmt, args) __attribute__((format(func, fmt, args)))

/**
 * @brief Cast pointer with specific offset.
 *
 */
#define pcast_offs(ptr, offs) ((void*)((uintptr_t)(ptr) + (offs))) // NOLINT

/**
 * @brief Exec asm commands.
 *
 */
#define asm_exec(...) __asm__ __volatile__(__VA_ARGS__)

/**
 * @brief Align up number.
 *
 */
#define align_up(num, align) (((num) + ((align) - 1)) & (~((align) - 1)))

#define likely(...) __builtin_expect(!!(__VA_ARGS__), 1)

#define unlikely(...) __builtin_expect(!!(__VA_ARGS__), 0)
