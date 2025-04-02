#pragma once

#include "kernel/config/filesystem.h"
#include "kernel/device/disk/partition.h"
#include "lib/types.h"

enum file_type : u32
{
  FT_REGULAR,
  FT_DIRECTORY,
  FT_SYMLINK
};

struct superblock
{
  u32 magic;
  u32 sec_cnt;
  u32 inode_cnt;
  u32 sec_start;

  u32 block_bitmap_start;
  u32 block_bitmap_sec_cnt;

  u32 inode_bitmap_start;
  u32 inode_bitmap_sec_cnt;

  u32 inode_table_start;
  u32 inode_table_sec_cnt;

  u32 data_start;
  u32 root_inode_idx;
  u32 direntry_size;

  u8 other[460];
} __attribute__((packed));

// struct dir
// {
//   struct inode* inode;
//   u32 pos;
//   u8 buf[FS_DIRBUFF_SIZE];
// };

struct direntry
{
  u32 iid;
  char name[FS_FILENAME_SIZE];
  enum file_type type;
};

void
init_fs(void);

bool
fs_mount(struct disk_partition* part);

void
fs_umount(struct disk_partition* part);