#pragma once

#include "kernel/device/disk/ide.h"
#include "kernel/memory/memory.h"
#include "lib/types.h"

/**
 * @brief Hard disk device
 */
struct disk
{
  char name[DEVICE_DISK_NAME_SIZE];
  struct ide_channel* channel;
  size_t dev_id;
  size_t sec_cnt;
};

/**
 * @brief Initialize disk device
 */
void
init_disk(void);

/**
 * @brief Get index of disk
 *
 * @param disk disk pointer
 * @return size_t disk index
 */
size_t
disk_get_index(struct disk* disk);

/**
 * @brief Get disk by index
 *
 * @param index disk index
 * @return struct disk* disk pointer
 */
struct disk*
disk_get(size_t index);

/**
 * @brief Read data from disk
 *
 * @param disk disk pointer
 * @param sec_start sector start
 * @param sec_cnt sector count
 * @param buf buffer pointer
 */
void
disk_read(struct disk* disk, u32 sec_start, u32 sec_cnt, void* buf);

/**
 * @brief Get disk count
 *
 * @return u8 disk count
 */
static FORCE_INLINE u8
disk_get_cnt(void)
{
  return *(u8*)MEM_KERNEL_VADDR(0x475);
}