#include "kernel/utils/bitmap.h"
#include "kernel/assert.h"
#include "lib/types.h"

void
ktester(void);

void
ktester(void)
{
  u8 bitmap_zones[8] = { 0 };
  struct bitmap bitmap;

  // Test bitmap_init
  bitmap_init(&bitmap, bitmap_zones, 8);
  KASSERT_EXPR(bitmap.data == bitmap_zones);
  KASSERT_EXPR(bitmap.size == 8);

  // Test bitmap_set
  bitmap_set(&bitmap, 0, true);
  KASSERT_EXPR(bitmap.data[0] == 0x01);

  // Test bitmap_get
  KASSERT(bitmap_get(&bitmap, 0) == true,
          "bitmap_get failed because get(0) is incorrect, expected 1 got 0");

  // Test bitmap_find
  KASSERT(bitmap_find(&bitmap, 1, true) == 0,
          "bitmap_find failed because find(1, true) is not found");
  KASSERT_EXPR(bitmap_find(&bitmap, 10, false) == 1);
  KASSERT_EXPR(bitmap_find(&bitmap, 10, true) == -1);

  // Test bitmap_alloc
  KASSERT_EXPR(bitmap_alloc(&bitmap, 10) == 1);
  KASSERT_EXPR(bitmap.data[0] == 0xFF);
  KASSERT_EXPR(bitmap.data[1] == 0x07);
}