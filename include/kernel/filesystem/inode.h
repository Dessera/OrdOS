#pragma once

#include "kernel/boot.h"
#include "kernel/device/disk/partition.h"
#include "kernel/filesystem/filesystem.h"
#include "lib/types.h"

struct inode_position
{
  u32 sec;
  u32 offset;
};

void
inode_init(struct inode* inode, u32 iid);

void
inode_locate(struct inode_position* pos, struct disk_partition* part, u32 iid);

bool
inode_sync(struct inode* inode, struct disk_partition* part);

struct inode*
inode_open(struct disk_partition* part, u32 iid);

void
inode_close(struct inode* inode);

static FORCE_INLINE bool
inode_pos_is_overflow(struct inode_position* pos)
{
  return pos->offset + (sizeof(struct inode)) >= BOOT_SEC_SIZE;
}