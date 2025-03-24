#pragma once

#include "kernel/memory/memory.h"
#include "lib/types.h"

#define DISK_NAME_SIZE 8

#define DISK_PARTITION_NAME_SIZE 8

#define DISK_CNT_ADDR 0x475

struct ide_channel;
struct disk;

struct disk_partition
{
  char name[DISK_PARTITION_NAME_SIZE];
  size_t sec_start;
  size_t sec_cnt;

  struct disk* disk;

  // TODO: file system
};

struct disk
{
  char name[DISK_NAME_SIZE];
  struct ide_channel* channel;
  size_t dev_id;

  struct disk_partition partitions[4];
  struct disk_partition logical_partitions[8];
};

void
init_disk(void);

void
disk_identify(struct disk* disk);

static FORCE_INLINE u8
disk_get_cnt(void)
{
  return *(u8*)MEM_KERNEL_VADDR(DISK_CNT_ADDR);
}