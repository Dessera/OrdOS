#pragma once

#include "kernel/config/filesystem.h"
#include "lib/list_head.h"
#include "lib/types.h"

ENUM_DECLARE(file_type, FT_REGULAR, FT_DIRECTORY, FT_SYMLINK);

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

struct inode
{
  u32 iid;
  u32 size;
  u32 ref_cnt;
  u32 direct[12];
  u32 indirect;
  struct list_head node;
};

struct dir
{
  struct inode* inode;
  u32 pos;
  u8 buf[FS_DIRBUFF_SIZE];
};

struct direntry
{
  uint32_t inode_idx;
  uint8_t name[FS_FILENAME_SIZE];
  enum file_type type;
};