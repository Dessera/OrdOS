#include "kernel/utils/bitmap.h"
#include "kernel/assert.h"
#include "lib/string.h" // IWYU pragma: keep
#include "lib/types.h"

void
bitmap_init(struct bitmap* bitmap, void* data, size_t byte_size)
{
  KASSERT_WARN(byte_size < MAX_SSIZE_T / 8,
               "bitmap init with too large size: %u",
               byte_size);

  bitmap->size = byte_size;
  bitmap->data = data;
  memset(bitmap->data, 0, byte_size);
}

bool
bitmap_get(struct bitmap* bitmap, size_t index)
{
  return bitmap->data[index / 8] & (1 << (index % 8));
}

void
bitmap_set(struct bitmap* bitmap, size_t index, bool value)
{
  if (value) {
    bitmap->data[index / 8] |= (1 << (index % 8));
  } else {
    bitmap->data[index / 8] &= ~(1 << (index % 8));
  }
}

ssize_t
bitmap_find(struct bitmap* bitmap, size_t size, bool value)
{
  size_t total_bit_size = bitmap->size * 8;

  size_t start = 0;
  while (start < total_bit_size) {
    size_t end = start;
    while (end < total_bit_size && bitmap_get(bitmap, end) == value) {
      end++;

      if (end - start == size) {
        KASSERT_WARN(end - 1 <= MAX_SSIZE_T || start <= MAX_SSIZE_T,
                     "bitmap find index out of range, start=%u, end=%u, max=%d",
                     start,
                     end,
                     MAX_SSIZE_T);
        if (end - 1 > MAX_SSIZE_T || start > MAX_SSIZE_T) {
          return NPOS;
        }
        return start;
      }
    }

    start = end + 1;
  }

  return NPOS;
}

ssize_t
bitmap_alloc(struct bitmap* bitmap, size_t size)
{
  ssize_t index = bitmap_find(bitmap, size, false);
  if (index != NPOS) {
    for (size_t i = 0; i < size; i++) {
      bitmap_set(bitmap, index + i, true);
    }
  }

  return index;
}