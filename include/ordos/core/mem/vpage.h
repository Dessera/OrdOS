/**
 * @file vpage.h
 * @author Dessera (dessera@qq.com)
 * @brief Pagetable utils.
 * @version 0.1.0
 * @date 2025-08-23
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/kernel/config.h"
#include "ordos/lib/asm.h"
#include "ordos/lib/types.h"

#define VPAGE_DESC_SIZE 4 /**< Page entry size. */

#define VPAGE_DESC_CNT                                                         \
  (ORDOS_KERNEL_PAGE_SIZE /                                                    \
   VPAGE_DESC_SIZE) /**< Page entries length for a page table. */

#define VPAGE_CR3_FLAG 0x80000000 /**< CR3 flag mask. */

/**
 * @brief Get pde descriptor from base and flags.
 *
 */
#define pde_desc(base, flags) (((uintptr_t)(base)) | (flags))

/**
 * @brief Get pte descriptor from base and flags.
 *
 */
#define pte_desc(base, flags) (((uintptr_t)(base)) | (flags))

/**
 * @brief Get pde index from addr.
 *
 */
#define pde_index(addr) (((uintptr_t)(addr)) >> 22)

/**
 * @brief Get pte index from addr.
 *
 */
#define pte_index(addr) ((((uintptr_t)(addr)) >> 12) & 0x3FF)

/**
 * @brief PDE type.
 *
 */
typedef u32 pde_t;

/**
 * @brief PTE type.
 *
 */
typedef u32 pte_t;

/**
 * @brief Page table flags.
 *
 */
enum vpage_flag
{
  PG_PRESENT = 0b1,     /**< Page is available. */
  PG_WRITABLE = 0b10,   /**< Page is writable. */
  PG_KERNEL = 0b100,    /**< Page is for kernel space. */
  PG_ACCESS = 0b100000, /**< Page is accessed. */
  PG_DIRTY = 0b1000000, /**< Page is dirty. */
};

/**
 * @brief Load page table.
 *
 * @param addr Page table pointer.
 */
__inline static void
vpage_load(void* addr)
{
  lcr3(addr);
}

/**
 * @brief Enable paging.
 *
 */
__inline static void
vpage_enable(void)
{
  lcr0(rcr0() | VPAGE_CR3_FLAG);
}
