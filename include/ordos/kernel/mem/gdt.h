/**
 * @file gdt.h
 * @author Dessera (dessera@qq.com)
 * @brief Kernel GDT.
 * @version 0.1.0
 * @date 2025-08-23
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/kernel/config.h"
#include "ordos/kernel/flag.h" // IWYU pragma: keep
#include "ordos/lib/common.h"  // IWYU pragma: keep
#include "ordos/lib/types.h"

#define GDT_NULL_INDEX 0  /**< GDT NULL descriptor index. */
#define GDT_KCODE_INDEX 1 /**< GDT KCODE descriptor index. */
#define GDT_KDATA_INDEX 2 /**< GDT KDATA descriptor index. */
#define GDT_UCODE_INDEX 3 /**< GDT UCODE descriptor index. */
#define GDT_UDATA_INDEX 4 /**< GDT UDATA descriptor index. */

/**
 * @brief Util to create GDT selector for registers.
 *
 */
#define gdt_create_sel(index, dpl) (((index) << 3) | dpl)

/**
 * @brief GDT KCODE selector.
 *
 */
#define gdt_sel_kcode() gdt_create_sel(GDT_KCODE_INDEX, DPL_KERNEL)

/**
 * @brief GDT KDATA selector.
 *
 */
#define gdt_sel_kdata() gdt_create_sel(GDT_KDATA_INDEX, DPL_KERNEL)

/**
 * @brief GDT UCODE selector.
 *
 */
#define gdt_sel_ucode() gdt_create_sel(GDT_UCODE_INDEX, DPL_USER)

/**
 * @brief GDT UDATA selector.
 *
 */
#define gdt_sel_udata() gdt_create_sel(GDT_UDATA_INDEX, DPL_USER)

/**
 * @brief Util to create GDT descriptor, which should be statically initialized.
 *
 */
#define gdt_desc(base, limit, flags)                                           \
  {                                                                            \
    .limit_l16 = (limit) & 0xFFFF,                                             \
    .base_l16 = (base) & 0xFFFF,                                               \
    .base_m8 = ((base) >> 16) & 0xFF,                                          \
    .attr = (flags) & 0xFF,                                                    \
    .attr_limit_h4 = (((flags) >> 8) & 0xFF) + (((limit) >> 16) & 0xF),        \
    .base_h8 = ((base) >> 24) & 0xFF,                                          \
  }

/**
 * @brief GDT NULL descriptor.
 *
 */
#define gdt_desc_null() gdt_desc(0, 0, GDT_KERNEL)

/**
 * @brief GDT KCODE descriptor.
 *
 */
#define gdt_desc_kcode()                                                       \
  gdt_desc(0, 0xFFFFF, GDT_PRESENT | GDT_KERNEL | GDT_CODE | GDT_G | GDT_D)

/**
 * @brief GDT KDATA descriptor.
 *
 */
#define gdt_desc_kdata()                                                       \
  gdt_desc(0, 0xFFFFF, GDT_PRESENT | GDT_KERNEL | GDT_DATA | GDT_G | GDT_D)

/**
 * @brief GDT UCODE descriptor.
 *
 */
#define gdt_desc_ucode()                                                       \
  gdt_desc(0, 0xFFFFF, GDT_PRESENT | GDT_USER | GDT_DATA | GDT_G | GDT_D)

/**
 * @brief GDT UDATA descriptor.
 *
 */
#define gdt_desc_udata()                                                       \
  gdt_desc(0, 0xFFFFF, GDT_PRESENT | GDT_USER | GDT_DATA | GDT_G | GDT_D)

/**
 * @brief GDTR for `lgdt`.
 *
 */
typedef u64 gdtr_t;

/**
 * @brief GDT entry.
 *
 */
struct gdt
{
  u16 limit_l16;
  u16 base_l16;
  u8 base_m8;
  u8 attr;
  u8 attr_limit_h4;
  u8 base_h8;
};

/**
 * @brief GDT flags.
 *
 */
enum gdt_flag
{
  GDT_NULL = 0x0,     /**< GDT is NULL entry. */
  GDT_TSS = 0x09,     /**< GDT is TSS entry. */
  GDT_CODE = 0x1a,    /**< GDT is code section entry. */
  GDT_DATA = 0x12,    /**< GDT is data section entry. */
  GDT_KERNEL = 0x0,   /**< GDT is for kernel space. */
  GDT_USER = 0x60,    /**< GDT is for user space. */
  GDT_PRESENT = 0x80, /**< GDT is available. */
  GDT_L = 0x2000,     /**< GDT L flag. */
  GDT_D = 0x4000,     /**< GDT D flag. */
  GDT_G = 0x8000,     /**< GDT G flag. */
};

/**
 * @brief Kernel GDT.
 *
 */
extern struct gdt __gdt[ORDOS_MEM_GDT_DESC_CNT];

/**
 * @brief Util to create gdtr for `lgdt`.
 *
 * @param gdt GDT pointer.
 * @param size GDT count.
 * @return gdtr_t GDTR
 */
__inline static gdtr_t
gdt_create_ptr(struct gdt* gdt, size_t size)
{
  return (sizeof(struct gdt) * size - 1) | ((gdtr_t)(uintptr_t)(gdt) << WORD);
}
