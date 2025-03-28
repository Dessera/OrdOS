#pragma once

#include "kernel/config/memory.h"
#include "kernel/defs.h" // IWYU pragma: keep

#define GDT_GET_SELECTOR(index, dpl) (((index) << 3) | (dpl))

#define GDT_NULL_INDEX 0
#define GDT_KCODE_INDEX 1
#define GDT_KDATA_INDEX 2
#define GDT_VIDEO_INDEX 3
#define GDT_TSS_INDEX 4
#define GDT_UCODE_INDEX 5
#define GDT_UDATA_INDEX 6

#define GDT_KCODE_SELECTOR GDT_GET_SELECTOR(GDT_KCODE_INDEX, DPL_KERNEL)
#define GDT_KDATA_SELECTOR GDT_GET_SELECTOR(GDT_KDATA_INDEX, DPL_KERNEL)
#define GDT_KSTACK_SELECTOR GDT_KDATA_SELECTOR

#define GDT_VIDEO_SELECTOR GDT_GET_SELECTOR(GDT_VIDEO_INDEX, DPL_KERNEL)

#define GDT_TSS_SELECTOR GDT_GET_SELECTOR(GDT_TSS_INDEX, DPL_KERNEL)
#define GDT_UCODE_SELECTOR GDT_GET_SELECTOR(GDT_UCODE_INDEX, DPL_USER)
#define GDT_UDATA_SELECTOR GDT_GET_SELECTOR(GDT_UDATA_INDEX, DPL_USER)

#ifndef __ASSEMBLER__

#include "kernel/task/context.h" // IWYU pragma: keep
#include "lib/types.h"

enum gdt_desc_type : u8
{
  GDT_DESC_TYPE_X = 0x08,
  GDT_DESC_TYPE_R = 0x04,
  GDT_DESC_TYPE_C = 0x02,
  GDT_DESC_TYPE_A = 0x01
};

/**
 * @brief GDT descriptor type
 */
struct gdt_desc
{
  u16 limit_l16;
  u16 base_l16;
  u8 base_m8;
  u8 attr;
  u8 attr_limit_h4;
  u8 base_h8;
};

extern struct gdt_desc gdt[MEM_GDT_SIZE];

#define GDT_DESC(limit, base, p, dpl, s, type, g, d, l)                        \
  (struct gdt_desc)                                                            \
  {                                                                            \
    ((limit) & 0xffff), ((base) & 0xffff), (((base) >> 16) & 0xff),            \
      ((p) << 7) + ((dpl) << 5) + ((s) << 4) + (type),                         \
      (u8)(((g) << 7) + ((d) << 6) + ((l) << 5) + (((limit) >> 16) & 0xf)),    \
      (((base) >> 24) & 0xff)                                                  \
  }

/**
 * @brief GDT Null descriptor
 */
#define GDT_DESC_NULL() GDT_DESC(0, 0, NOT_PRESENT, DPL_KERNEL, 0, 0, 0, 0, 0)

/**
 * @brief GDT Kernel code segment descriptor (temporary)
 */
#define GDT_DESC_TCODE()                                                       \
  GDT_DESC(0xfffff,                                                            \
           -MEM_KERNEL_VSTART,                                                 \
           PRESENT,                                                            \
           DPL_KERNEL,                                                         \
           1,                                                                  \
           GDT_DESC_TYPE_X | GDT_DESC_TYPE_C,                                  \
           1,                                                                  \
           1,                                                                  \
           0)

/**
 * @brief GDT Kernel data segment descriptor (temporary)
 */
#define GDT_DESC_TDATA()                                                       \
  GDT_DESC(0xfffff,                                                            \
           -MEM_KERNEL_VSTART,                                                 \
           PRESENT,                                                            \
           DPL_KERNEL,                                                         \
           1,                                                                  \
           GDT_DESC_TYPE_C,                                                    \
           1,                                                                  \
           1,                                                                  \
           0)

/**
 * @brief GDT Kernel code segment descriptor
 */
#define GDT_DESC_KCODE()                                                       \
  GDT_DESC(0xfffff,                                                            \
           0,                                                                  \
           PRESENT,                                                            \
           DPL_KERNEL,                                                         \
           1,                                                                  \
           GDT_DESC_TYPE_X | GDT_DESC_TYPE_C,                                  \
           1,                                                                  \
           1,                                                                  \
           0)

/**
 * @brief GDT Kernel data segment descriptor
 */
#define GDT_DESC_KDATA()                                                       \
  GDT_DESC(0xfffff, 0, PRESENT, DPL_KERNEL, 1, GDT_DESC_TYPE_C, 1, 1, 0)

/**
 * @brief GDT User code segment descriptor
 */
#define GDT_DESC_UCODE()                                                       \
  GDT_DESC(0xfffff,                                                            \
           0,                                                                  \
           PRESENT,                                                            \
           DPL_USER,                                                           \
           1,                                                                  \
           GDT_DESC_TYPE_X | GDT_DESC_TYPE_C,                                  \
           1,                                                                  \
           1,                                                                  \
           0)

/**
 * @brief GDT User data segment descriptor
 */
#define GDT_DESC_UDATA()                                                       \
  GDT_DESC(0xfffff, 0, PRESENT, DPL_USER, 1, GDT_DESC_TYPE_C, 1, 1, 0)

/**
 * @brief GDT Video segment descriptor (temporary)
 */
#define GDT_DESC_TVIDEO()                                                      \
  GDT_DESC(0x07, 0xb8000, PRESENT, DPL_KERNEL, 1, GDT_DESC_TYPE_C, 1, 1, 0)

/**
 * @brief GDT Video segment descriptor
 */
#define GDT_DESC_VIDEO()                                                       \
  GDT_DESC(0x07,                                                               \
           0xb8000 + MEM_KERNEL_VSTART,                                        \
           PRESENT,                                                            \
           DPL_KERNEL,                                                         \
           1,                                                                  \
           GDT_DESC_TYPE_C,                                                    \
           1,                                                                  \
           1,                                                                  \
           0)

/**
 * @brief GDT TSS segment descriptor
 */
#define GDT_TSS_DESC(ctx)                                                      \
  GDT_DESC(sizeof(struct tss_context),                                         \
           (u32)ctx,                                                           \
           PRESENT,                                                            \
           DPL_KERNEL,                                                         \
           0,                                                                  \
           GDT_DESC_TYPE_X | GDT_DESC_TYPE_A,                                  \
           1,                                                                  \
           0,                                                                  \
           0)

/**
 * @brief Get GDT register pointer
 *
 * @param gdt GDT pointer
 * @param size GDT size
 * @return u64 GDT register pointer
 */
static FORCE_INLINE u64
gdt_get_ptr(struct gdt_desc* gdt, size_t size)
{
  return (sizeof(struct gdt_desc) * size - 1) | ((u64)(uintptr_t)(gdt) << 16);
}

#endif
