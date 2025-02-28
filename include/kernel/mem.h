#pragma once

#define E820MAGIC 0x534d4150
#define E820FUNC 0x0000e820

#define PAGE_SIZE 4096
#define PAGE_TABLE_ADDR 0x100000 // 1MB
#define INDEX_PAGE_TABLE_ADDR(index) ((index) * 0x1000 + PAGE_TABLE_ADDR)

#define PDE_P(sign) (sign)
#define PDE_RW(sign) (sign << 1)
#define PDE_US(sign) (sign << 2)

#define PDE_DESC(base, p, w, u) ((base) | PDE_P(p) | PDE_RW(w) | PDE_US(u))
