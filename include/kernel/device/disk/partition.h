#pragma once

#include "kernel/device/disk/disk.h"
#include "kernel/filesystem/filesystem.h"
#include "kernel/utils/bitmap.h"
#include "lib/list_head.h"

struct disk_partition
{
  char name[DEVICE_DISK_PARTITION_NAME_SIZE];
  size_t sec_start;
  size_t sec_cnt;
  bool logical;

  struct disk* disk;
  struct list_head node;

  // TODO: file system
  struct superblock* sb;
  struct bitmap block_bitmap;
  struct bitmap inode_bitmap;
  struct list_head open_inodes;
};

void
init_partition(void);

struct disk_partition*
partition_get_by_name(const char* name);

struct disk_partition*
partition_get_by_index(size_t index);

size_t
partition_get_index(struct disk_partition* partition);

size_t
partition_get_cnt(void);