#include "ordos/lib/util/bitmap.h"
#include "ordos/lib/string.h" // IWYU pragma: keep
#include "ordos/lib/types.h"

void
bitmap_init(struct bitmap* bitmap, void* data, size_t byte_size)
{
  bitmap->size = byte_size;
  bitmap->data = data;
  memset(bitmap->data, 0, byte_size);
}

bool
bitmap_get(struct bitmap* bitmap, size_t index)
{
  return bitmap->data[index / BYTE] & (1 << (index % BYTE));
}

void
bitmap_set(struct bitmap* bitmap, size_t index, bool value)
{
  if (value) {
    bitmap->data[index / BYTE] |= (1 << (index % BYTE));
  } else {
    bitmap->data[index / BYTE] &= ~(1 << (index % BYTE));
  }
}

ssize_t
bitmap_find(struct bitmap* bitmap, size_t size, bool value)
{
  size_t total_bit_size = bitmap->size * BYTE;

  size_t start = 0;
  while (start < total_bit_size) {
    size_t end = start;
    while (end < total_bit_size && bitmap_get(bitmap, end) == value) {
      end++;

      if (end - start == size) {
        if (end - 1 > ISIZE_MAX || start > ISIZE_MAX) {
          return NPOS;
        }
        return (ssize_t)start;
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
