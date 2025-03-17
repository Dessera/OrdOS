#pragma once

#include "kernel/config/memory.h"

#define GDT_G(sign) ((sign) << 7)
#define GDT_G_1B GDT_G(0)
#define GDT_G_4K GDT_G(1)

#define GDT_D(sign) ((sign) << 6)
#define GDT_D_16 GDT_D(0)
#define GDT_D_32 GDT_D(1)

#define GDT_L(sign) ((sign) << 5)
#define GDT_L_32 GDT_L(0)
#define GDT_L_64 GDT_L(1)

#define GDT_AVL(sign) ((sign) << 4)

#define GDT_P(sign) ((sign) << 7)

#define GDT_DPL(sign) ((sign) << 5)

#define GDT_S(sign) ((sign) << 4)

#define GDT_TYPE(x, r, c, a) (((x) << 3) | ((r) << 2) | ((c) << 1) | (a))

#define GDT_LIMIT_L16(limit) ((limit) & 0xffff)
#define GDT_LIMIT_H4(limit) (((limit) >> 16) & 0xf)

#define GDT_BASE_L16(base) ((base) & 0xffff)
#define GDT_BASE_M8(base) (((base) >> 16) & 0xff)
#define GDT_BASE_H8(base) (((base) >> 24) & 0xff)

#define GDT_GET_SELECTOR(index, dpl) (((index) << 3) | (dpl))

#define GDT_NULL_INDEX 0
#define GDT_KCODE_INDEX 1
#define GDT_KDATA_INDEX 2
#define GDT_VIDEO_INDEX 3
#define GDT_TSS_INDEX 4
#define GDT_UCODE_INDEX 5
#define GDT_UDATA_INDEX 6

#define GDT_KCODE_SELECTOR GDT_GET_SELECTOR(GDT_KCODE_INDEX, 0)
#define GDT_KDATA_SELECTOR GDT_GET_SELECTOR(GDT_KDATA_INDEX, 0)
#define GDT_KSTACK_SELECTOR GDT_KDATA_SELECTOR

#define GDT_VIDEO_SELECTOR GDT_GET_SELECTOR(GDT_VIDEO_INDEX, 0)

#define GDT_TSS_SELECTOR GDT_GET_SELECTOR(GDT_TSS_INDEX, 0)
#define GDT_UCODE_SELECTOR GDT_GET_SELECTOR(GDT_UCODE_INDEX, 3)
#define GDT_UDATA_SELECTOR GDT_GET_SELECTOR(GDT_UDATA_INDEX, 3)

#ifndef __ASM__

#include "lib/types.h"

struct gdt_desc
{
  u16 limit_l16;
  u16 base_l16;
  u8 base_m8;
  u8 attr;
  u8 attr_limit_h4;
  u8 base_h8;
};

struct gdt_ptr
{
  u16 limit;
  u32 base;
};

extern struct gdt_desc gdt[MEM_GDT_SIZE];

#define GDT_DESC(limit, base, p, dpl, s, x, r, c, a, g, d, l, avl)             \
  { GDT_LIMIT_L16(limit),                                                      \
    GDT_BASE_L16(base),                                                        \
    GDT_BASE_M8(base),                                                         \
    GDT_P(p) + GDT_DPL(dpl) + GDT_S(s) + GDT_TYPE(x, r, c, a),                 \
    (u8)(GDT_G(g) + GDT_D(d) + GDT_L(l) + GDT_AVL(avl) + GDT_LIMIT_H4(limit)), \
    GDT_BASE_H8(base) }

#define GDT_DESC_NULL() GDT_DESC(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

#define GDT_DESC_TCODE()                                                       \
  GDT_DESC(0xfffff, -MEM_KERNEL_VSTART, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0)

#define GDT_DESC_TDATA()                                                       \
  GDT_DESC(0xfffff, -MEM_KERNEL_VSTART, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0)

#define GDT_DESC_KCODE() GDT_DESC(0xfffff, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0)

#define GDT_DESC_KDATA() GDT_DESC(0xfffff, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0)

#define GDT_DESC_UCODE() GDT_DESC(0xfffff, 0, 1, 3, 1, 1, 0, 1, 0, 1, 1, 0, 0)

#define GDT_DESC_UDATA() GDT_DESC(0xfffff, 0, 1, 3, 1, 0, 0, 1, 0, 1, 1, 0, 0)

#define GDT_DESC_TVIDEO()                                                      \
  GDT_DESC(0x07, 0xb8000, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0)

#define GDT_DESC_VIDEO()                                                       \
  GDT_DESC(0x07, 0xb8000 + MEM_KERNEL_VSTART, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0)

#define GDT_GET_PTR(size, addr)                                                \
  ((sizeof(struct gdt_desc) * (size) - 1) | ((u64)(u32)(addr) << 16))

#endif
