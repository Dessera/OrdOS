/**
 * @file section.h
 * @author Dessera (dessera@qq.com)
 * @brief Kernel section utils.
 * @version 0.1.0
 * @date 2025-08-28
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/lib/common.h"

/**
 * @brief Tell compiler the function is used by prelude.
 *
 */
#define __prelude __section(".text.init")

/**
 * @brief Tell compiler the variable is used by prelude.
 *
 */
#define __prelude_data __section(".data.init")

/**
 * @brief Tell compiler the variable is a module.
 *
 */
#define __module __section(".data.module")