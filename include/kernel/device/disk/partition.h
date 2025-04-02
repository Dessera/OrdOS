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

  struct superblock* sb;
  struct bitmap block_bitmap;
  struct bitmap inode_bitmap;
  struct list_head open_inodes;
};

struct partition_table_entry
{
  u8 bootable;
  u8 start_head;
  u8 start_sec;
  u8 start_chs;
  u8 fs_type;
  u8 end_head;
  u8 end_sec;
  u8 end_chs;
  u32 start_lba;
  u32 sec_cnt;
} __attribute__((packed));

struct boot_sector
{
  u8 other[446];
  struct partition_table_entry partition_table[4];
  u16 signature;
} __attribute__((packed));

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