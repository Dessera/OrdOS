/**
 * @file init.h
 * @author Dessera (dessera@qq.com)
 * @brief Common kernel initialization info.
 * @version 0.1.0
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/core/mem/mmap.h"
#include "ordos/core/mem/vpage.h"
#include "ordos/kernel/config.h"
#include "ordos/lib/types.h" // IWYU pragma: keep

/**
 * @brief Init information for kernel.
 *
 */
struct init_info
{
  struct mmap_entry mmap[ORDOS_INIT_MMAP_CNT]; /**< Memory map. */
  size_t mmap_cnt;                             /**< Memory map count. */
  pde_t* kernel_pd;                            /**< Kernel page directory. */
  size_t kernel_pde_cnt;                       /**< Kernel pde count. */
  pte_t* kernel_pt;                            /**< Kernel page table. */
  size_t kernel_pte_cnt;                       /**< Kernel pte count. */
  char cmdline[ORDOS_INIT_ARGS_BUFSIZE];       /**< Boot args. */
};

extern struct init_info __init; /**< Global init info. */
