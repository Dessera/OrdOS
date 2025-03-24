#pragma once

#define DISK_PORT_DATA 0x1f0

#define DISK_CMD_SECS 0x1f2
#define DISK_CMD_LBALOW 0x1f3
#define DISK_CMD_LBAMID 0x1f4
#define DISK_CMD_LBAHIGH 0x1f5
#define DISK_CMD_LBAEX 0x1f6
#define DISK_CMD_CTRL 0x1f7

#define DISK_CMD_READ 0x20
#define DISK_CMD_WRITE 0x30
#define DISK_CMD_IDENTIFY 0xec

#define DISK_STATUS_BUSY 0x80
#define DISK_STATUS_READY 0x40

#define DISK_IDE_CHANNEL_NAME_SIZE 8
#define DISK_NAME_SIZE 8
#define DISK_PARTITION_NAME_SIZE 8

#ifndef __ASSEMBLER__

#include "kernel/task/sync.h"
#include "lib/types.h"

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
  bool slave;

  struct disk_partition partitions[4];
  struct disk_partition logical_partitions[8];
};

struct ide_channel
{
  char name[DISK_IDE_CHANNEL_NAME_SIZE];
  u16 base_port;
  u32 irq;

  struct mutex_lock lock;
  struct semaphore irq_sem;

  bool irq_ready;
  struct disk devices[2];
};

#endif