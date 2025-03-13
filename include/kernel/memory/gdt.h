#pragma once

#ifndef __ASM__
#include "kernel/types.h"
#endif

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

#ifdef __ASM__
#define GDT_DESC(limit, base, p, dpl, s, x, r, c, a, g, d, l, avl)             \
  .word GDT_LIMIT_L16(limit);                                                  \
  .word GDT_BASE_L16(base);                                                    \
  .byte GDT_BASE_M8(base);                                                     \
  .byte GDT_P(p) + GDT_DPL(dpl) + GDT_S(s) + GDT_TYPE(x, r, c, a);             \
  .byte GDT_G(g) + GDT_D(d) + GDT_L(l) + GDT_AVL(avl) + GDT_LIMIT_H4(limit);   \
  .byte GDT_BASE_H8(base)
#else

struct gdt_desc
{
  u16 limit_l16;
  u16 base_l16;
  u8 base_m8;
  u8 attrs;
  u8 limit_h4;
  u8 base_h8;
} __attribute__((packed));

#define GDT_DESC(limit, base, p, dpl, s, x, r, c, a, g, d, l, avl)             \
  ({                                                                           \
    struct gdt_desc desc = { GDT_LIMIT_L16(limit),                             \
                             GDT_BASE_L16(base),                               \
                             GDT_BASE_M8(base),                                \
                             GDT_P(p) + GDT_DPL(dpl) + GDT_S(s) +              \
                               GDT_TYPE(x, r, c, a),                           \
                             (u8)(GDT_G(g) + GDT_D(d) + GDT_L(l) +             \
                                  GDT_AVL(avl) + GDT_LIMIT_H4(limit)),         \
                             GDT_BASE_H8(base) };                              \
    desc;                                                                      \
  })

#endif

#define GDT_DESC_NULL() GDT_DESC(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

#define GDT_DESC_KCODE() GDT_DESC(0xfffff, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0)

#define GDT_DESC_KDATA() GDT_DESC(0xfffff, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0)

#define GDT_DESC_UCODE() GDT_DESC(0xfffff, 0, 1, 3, 1, 1, 0, 1, 0, 1, 1, 0, 0)

#define GDT_DESC_UDATA() GDT_DESC(0xfffff, 0, 1, 3, 1, 0, 0, 1, 0, 1, 1, 0, 0)

#define GDT_DESC_VIDEO()                                                       \
  GDT_DESC(0x07, 0xb8000, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0)

#define GDT_KCODE_SELECTOR 0x08
#define GDT_KDATA_SELECTOR 0x10
#define GDT_KSTACK_SELECTOR GDT_KDATA_SELECTOR

#define GDT_VIDEO_SELECTOR 0x18

#define GDT_TSS_SELECTOR 0x20
#define GDT_UCODE_SELECTOR 0x2b
#define GDT_UDATA_SELECTOR 0x33