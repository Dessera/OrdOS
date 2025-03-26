#pragma once

#include "kernel/device/disk/ide.h"
#include "kernel/memory/memory.h"
#include "lib/types.h"

#define DISK_NAME_SIZE 8

#define DISK_CNT_ADDR 0x475

struct disk
{
  char name[DISK_NAME_SIZE];
  struct ide_channel* channel;
  size_t dev_id;
  size_t sec_cnt;

  // struct disk_partition partitions[4];
  // struct disk_partition logical_partitions[8];
};

void
init_disk(void);

void
disk_read(struct disk* disk, u32 sec_start, u32 sec_cnt, void* buf);

static FORCE_INLINE u8
disk_get_cnt(void)
{
  return *(u8*)MEM_KERNEL_VADDR(DISK_CNT_ADDR);
}