#pragma once

#define IDE_CHANNEL_NAME_SIZE 8
#define IDE_CHANNEL_MAX_CNT 2

#define IDE0_BASE_PORT 0x1f0
#define IDE1_BASE_PORT 0x170

#ifndef __ASSEMBLER__

#include "kernel/assert.h"
#include "kernel/device/disk/disk.h"
#include "kernel/task/sync.h"
#include "lib/types.h"

struct ide_channel
{
  char name[IDE_CHANNEL_NAME_SIZE];
  u16 base_port;
  u32 irq;

  struct mutex_lock lock;
  struct semaphore irq_sem;

  bool irq_ready;
  struct disk devices[2];
};

KSTATIC_ASSERT(IDE_CHANNEL_MAX_CNT == 2, "only two ide channels supported");

enum ide_dev_bit : u8
{
  IDE_DEV_BIT_MBS = 0xa0,
  IDE_DEV_BIT_LBA = 0x40,
  IDE_DEV_BIT_DEV = 0x10,
};

enum ide_cmd : u8
{
  IDE_CMD_IDENTIFY = 0xec,
};

enum ide_status_bit : u8
{
  IDE_STATUS_BIT_BSY = 0x80,
  IDE_STATUS_BIT_DRDY = 0x40,
  IDE_STATUS_BIT_DRQ = 0x08,
};

void
init_ide(void);

size_t
ide_channel_get_index(struct ide_channel* channel);

struct ide_channel*
ide_channel_get(size_t index);

static FORCE_INLINE u16
ide_channel_data(struct ide_channel* channel)
{
  return channel->base_port;
}

static FORCE_INLINE u16
ide_channel_error(struct ide_channel* channel)
{
  return channel->base_port + 0x01;
}

static FORCE_INLINE u16
ide_channel_sec_cnt(struct ide_channel* channel)
{
  return channel->base_port + 0x02;
}

static FORCE_INLINE u16
ide_channel_lba_lo(struct ide_channel* channel)
{
  return channel->base_port + 0x03;
}

static FORCE_INLINE u16
ide_channel_lba_mid(struct ide_channel* channel)
{
  return channel->base_port + 0x04;
}

static FORCE_INLINE u16
ide_channel_lba_hi(struct ide_channel* channel)
{
  return channel->base_port + 0x05;
}

static FORCE_INLINE u16
ide_channel_dev(struct ide_channel* channel)
{
  return channel->base_port + 0x06;
}

static FORCE_INLINE u16
ide_channel_status(struct ide_channel* channel)
{
  return channel->base_port + 0x07;
}

static FORCE_INLINE u16
ide_channel_cmd(struct ide_channel* channel)
{
  return channel->base_port + 0x07;
}

static FORCE_INLINE u16
ide_channel_alt_status(struct ide_channel* channel)
{
  return channel->base_port + 0x206;
}

static FORCE_INLINE u16
ide_channel_dev_ctrl(struct ide_channel* channel)
{
  return channel->base_port + 0x206;
}

void
ide_select_device(struct ide_channel* channel, size_t dev_index);

void
ide_send_cmd(struct ide_channel* channel, enum ide_cmd cmd);

bool
ide_is_device_ready(struct ide_channel* channel);

void
ide_read_sector(struct ide_channel* channel, void* buffer, size_t sector);

#endif