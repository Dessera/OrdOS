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
#define __section(name) __attribute__((section(name)))

/**
 * @brief Tell compiler the function is used in somewhere.
 *
 */
#define __used __attribute__((used))

/**
 * @brief Tell compiler the function is used by asm code.
 *
 */
#define __asm_linkage __attribute__((regparm(0))) __used

/**
 * @brief Tell compiler the function should not return.
 *
 */
#define __noreturn __attribute__((noreturn))

/**
 * @brief Tell compiler the function is pure.
 *
 */
#define __pure __attribute__((pure))

/**
 * @brief Tell compiler the function should be inline expanded.
 *
 */
#define __inline inline __attribute__((always_inline))

/**
 * @brief Tell compiler check function format args.
 *
 */
#define __format(func, fmt, args) __attribute__((format(func, fmt, args)))

/**
 * @brief Cast pointer with specific offset.
 *
 */
#define poffset(ptr, offs) ((void*)((uintptr_t)(ptr) + (offs)))

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

/**
 * @brief Align down number.
 *
 */
#define align_down(num, align) ((num) & ~((align) - 1))

/**
 * @brief Divide up numbers.
 *
 */
#define div_up(x, y) (((x) + (y) - 1) / (y))

/**
 * @brief Divide down numbers.
 *
 */
#define div_down(x, y) ((x) / (y))

/**
 * @brief Get min value.
 *
 */
#define min(x, y) ((x) < (y) ? (x) : (y))

/**
 * @brief Get max value.
 *
 */
#define max(x, y) ((x) > (y) ? (x) : (y))

/**
 * @brief Tell compiler the condition is more likely to be true.
 *
 */
#define likely(...) __builtin_expect(!!(__VA_ARGS__), 1)

/**
 * @brief Tell compiler the condition is more likely to be false.
 *
 */
#define unlikely(...) __builtin_expect(!!(__VA_ARGS__), 0)
