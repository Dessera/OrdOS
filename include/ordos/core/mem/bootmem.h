/**
 * @file bootmem.h
 * @author Dessera (dessera@qq.com)
 * @brief Bootmem allocator.
 * @version 0.1.0
 * @date 2025-08-23
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/lib/section.h"
#include "ordos/lib/types.h" // IWYU pragma: keep

/**
 * @brief Init bootmem allocator.
 *
 */
__prelude void
bootmem_init(void);

/**
 * @brief Alloc mem from bootmem.
 *
 * @param size Memory size.
 * @return void* Memory pointer.
 */
__prelude void*
bootmem_alloc(size_t size);

/**
 * @brief Get bootmem start position (free space).
 *
 * @return uintptr_t Start position.
 */
__prelude uintptr_t
bootmem_start(void);

/**
 * @brief Get bootmem end position.
 *
 * @return uintptr_t End position.
 */
__prelude uintptr_t
bootmem_end(void);
