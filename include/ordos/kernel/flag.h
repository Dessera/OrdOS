/**
 * @file flag.h
 * @author Dessera (dessera@qq.com)
 * @brief Kernel common flags.
 * @version 0.1.0
 * @date 2025-08-23
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

/**
 * @brief Common DPL definition.
 *
 */
enum dpl
{
  DPL_KERNEL = 0, /**< Kernel privledge. */
  DPL_USER = 3,   /**< User privledge. */
};
