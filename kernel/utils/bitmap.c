#include "kernel/utils/bitmap.h"
#include "kernel/assert.h"
#include "kernel/task/sync.h"
#include "kernel/utils/string.h"
#include "lib/types.h"

void
bitmap_init(struct bitmap* bitmap, void* data, size_t byte_size)
{
  bitmap->size = byte_size;
  bitmap->data = data;
  kmemset(bitmap->data, 0, byte_size);
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
        KASSERT(end - 1 <= MAX_SSIZE_T || start <= MAX_SSIZE_T,
                "bitmap find index out of range, start=%u, end=%u, max=%d",
                start,
                end,
                MAX_SSIZE_T);
        return start;
      }
    }

    start = end + 1;
  }

  return -1;
}

ssize_t
bitmap_alloc(struct bitmap* bitmap, size_t size)
{
  ssize_t index = bitmap_find(bitmap, size, false);
  if (index != -1) {
    for (size_t i = 0; i < size; i++) {
      bitmap_set(bitmap, index + i, true);
    }
  }

  return index;
}

void
atomic_bitmap_init(struct atomic_bitmap* bitmap, void* data, size_t size)
{
  bitmap_init(&bitmap->bitmap, data, size);
  mutex_lock_init(&bitmap->lock);
}

void
atomic_bitmap_set(struct atomic_bitmap* bitmap, size_t index, bool value)
{
  mutex_lock(&bitmap->lock);
  bitmap_set(&bitmap->bitmap, index, value);
  mutex_unlock(&bitmap->lock);
}

bool
atomic_bitmap_get(struct atomic_bitmap* bitmap, size_t index)
{
  bool value;
  mutex_lock(&bitmap->lock);
  value = bitmap_get(&bitmap->bitmap, index);
  mutex_unlock(&bitmap->lock);
  return value;
}

ssize_t
atomic_bitmap_alloc(struct atomic_bitmap* bitmap, size_t size)
{
  ssize_t index;
  mutex_lock(&bitmap->lock);
  index = bitmap_alloc(&bitmap->bitmap, size);
  mutex_unlock(&bitmap->lock);
  return index;
}

ssize_t
atomic_bitmap_find(struct atomic_bitmap* bitmap, size_t size, bool value)
{
  ssize_t index;
  mutex_lock(&bitmap->lock);
  index = bitmap_find(&bitmap->bitmap, size, value);
  mutex_unlock(&bitmap->lock);
  return index;
}