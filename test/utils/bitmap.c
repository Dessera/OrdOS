#include "kernel/utils/bitmap.h"
#include "kernel/assert.h"
#include "kernel/types.h"

void
ktester(void);

u8 bitmap_zones[8] = { 0 };

void
ktester(void)
{
  struct bitmap bitmap;

  // Test bitmap_init
  bitmap_init(&bitmap, bitmap_zones, 8);
  KASSERT(bitmap.data == bitmap_zones);
  KASSERT(bitmap.size == 8);

  // Test bitmap_set
  bitmap_set(&bitmap, 0, true);
  KASSERT(bitmap.data[0] == 0x01);

  // Test bitmap_get
  KASSERT(bitmap_get(&bitmap, 0) == true);

  // Test bitmap_find
  KASSERT(bitmap_find(&bitmap, 1, true) == 0);
  KASSERT(bitmap_find(&bitmap, 10, false) == 1);
  KASSERT(bitmap_find(&bitmap, 10, true) == -1);

  // Test bitmap_alloc
  KASSERT(bitmap_alloc(&bitmap, 10) == 1);
  KASSERT(bitmap.data[0] == 0xFF);
  KASSERT(bitmap.data[1] == 0x07);
}