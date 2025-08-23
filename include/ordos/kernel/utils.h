/**
 * @file utils.h
 * @author Dessera (dessera@qq.com)
 * @brief Kernel specific utils.
 * @version 0.1.0
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/kernel/config.h"
#include "ordos/lib/common.h"

/**
 * @brief Access virtual memory space in physical section.
 *
 */
#define vaccess(ptr) (poffset(ptr, -ORDOS_KERNEL_VADDR))

/**
 * @brief Access physical memory space in vritual section.
 *
 */
#define paccess(ptr) (poffset(ptr, ORDOS_KERNEL_VADDR))
