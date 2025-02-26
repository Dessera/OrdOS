#pragma once

#define GDT_G(sign) (sign << 7)
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

#define GDT_DESC(limit_l16,                                                    \
                 limit_h4,                                                     \
                 base_l16,                                                     \
                 base_m8,                                                      \
                 base_h8,                                                      \
                 p,                                                            \
                 dpl,                                                          \
                 s,                                                            \
                 x,                                                            \
                 r,                                                            \
                 c,                                                            \
                 a,                                                            \
                 g,                                                            \
                 d,                                                            \
                 l,                                                            \
                 avl)                                                          \
  .word limit_l16;                                                             \
  .word base_l16;                                                              \
  .byte base_m8;                                                               \
  .byte GDT_P(p) + GDT_DPL(dpl) + GDT_S(s) + GDT_TYPE(x, r, c, a);             \
  .byte GDT_G(g) + GDT_D(d) + GDT_L(l) + GDT_AVL(avl) + limit_h4;              \
  .byte base_h8

#define GDT_DESC_NULL() GDT_DESC(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

#define GDT_DESC_CODE()                                                        \
  GDT_DESC(0xffff, 0xf, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0)

#define GDT_DESC_DATA()                                                        \
  GDT_DESC(0xffff, 0xf, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0)
#define GDT_DESC_VIDEO()                                                       \
  GDT_DESC(0x0007, 0xb, 0x8000, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0)

#define GDT_CODE_SELECTOR 0x08
#define GDT_DATA_SELECTOR 0x10