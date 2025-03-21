#pragma once

#include "lib/types.h"

/**
 * @brief Bitmap data structure
 */
struct bitmap
{
  size_t size;
  u8* data;
};

/**
 * @brief Initialize bitmap
 *
 * @param bitmap Bitmap to initialize
 * @param data Data pointer to use for bitmap
 * @param byte_size Size of data (in bytes)
 */
void
bitmap_init(struct bitmap* bitmap, void* data, size_t byte_size);

/**
 * @brief Get value of bit at index
 *
 * @param bitmap Bitmap to get value from
 * @param index Index of bit to get
 * @return true if bit is set
 * @return false if bit is not set
 */
bool
bitmap_get(struct bitmap* bitmap, size_t index);

/**
 * @brief Set value of bit at index
 *
 * @param bitmap Bitmap to set value in
 * @param index Index of bit to set
 * @param value Value to set bit to
 */
void
bitmap_set(struct bitmap* bitmap, size_t index, bool value);

/**
 * @brief Find first bit in bitmap that matches value
 *
 * @param bitmap Bitmap to search
 * @param size Size of bitmap
 * @param value Value to search for
 * @return ssize_t Index of first bit that matches value, or -1 if not found
 *
 * @note Index out of `ssize_t` range is considered not found
 */
ssize_t
bitmap_find(struct bitmap* bitmap, size_t size, bool value);

/**
 * @brief Allocate a block of bits in the bitmap
 *
 * @param bitmap Bitmap to allocate from
 * @param size Number of bits to allocate
 * @return ssize_t Index of first bit in allocated block, or -1 if allocation
 * failed
 *
 * @note Index out of `ssize_t` range is considered allocation failure
 */
ssize_t
bitmap_alloc(struct bitmap* bitmap, size_t size);