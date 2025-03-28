#pragma once

#define IDE_CHANNEL_MAX_CNT 2
#define IDE_CHANNEL_DEVICE_MAX_CNT 2

#define IDE0_BASE_PORT 0x1f0
#define IDE1_BASE_PORT 0x170

#define IDE0_SECCNT_PORT 0x1f2
#define IDE0_LBALO_PORT 0x1f3
#define IDE0_LBAMID_PORT 0x1f4
#define IDE0_LBAHI_PORT 0x1f5
#define IDE0_LBAEX_PORT 0x1f6
#define IDE0_CTRL_PORT 0x1f7

#ifndef __ASSEMBLER__

#include "kernel/config/device.h"
#include "kernel/device/disk/disk.h"
#include "kernel/task/sync.h"
#include "lib/types.h"

#define DEFINE_IDE_CHANNEL_PORT(name, offset)                                  \
  static FORCE_INLINE u16 name(struct ide_channel* channel)                    \
  {                                                                            \
    return channel->base_port + offset;                                        \
  }

struct ide_channel
{
  char name[DEVICE_IDE_CHANNEL_NAME_SIZE];
  u16 base_port;
  u32 irq;

  struct mutex_lock lock;

  struct semaphore irq_sem;
  bool irq_ready;
};

enum ide_dev_bit : u8
{
  IDE_DEV_BIT_MBS = 0xa0,
  IDE_DEV_BIT_LBA = 0x40,
  IDE_DEV_BIT_DEV = 0x10,
};

enum ide_cmd : u8
{
  IDE_CMD_IDENTIFY = 0xec,
  IDE_CMD_READ = 0x20,
  IDE_CMD_WRITE = 0x30,
};

enum ide_status_bit : u8
{
  IDE_STATUS_BIT_BSY = 0x80,
  IDE_STATUS_BIT_DRDY = 0x40,
  IDE_STATUS_BIT_DRQ = 0x08,
};

/**
 * @brief Initialize the IDE channels
 */
void
init_ide(void);

/**
 * @brief Get the index of the given IDE channel
 *
 * @param channel the IDE channel
 * @return size_t the index of the given IDE channel
 */
size_t
ide_channel_get_index(struct ide_channel* channel);

/**
 * @brief Get the IDE channel with the given index
 *
 * @param index the index of the IDE channel
 * @return struct ide_channel* the IDE channel with the given index
 */
struct ide_channel*
ide_channel_get(size_t index);

/**
 * @brief Select the given device on the given IDE channel
 *
 * @param channel the IDE channel
 * @param dev_index the device index
 */
void
ide_select_device(struct ide_channel* channel, size_t dev_index);

void
ide_select_sector(struct ide_channel* channel,
                  size_t dev_index,
                  size_t sec_start,
                  size_t sec_cnt);

/**
 * @brief Send a command to the given IDE channel
 *
 * @param channel the IDE channel
 * @param cmd the command to send
 * @warning This function will block until the interrupt semaphore is released
 */
void
ide_send_cmd(struct ide_channel* channel, enum ide_cmd cmd);

/**
 * @brief Check if the given IDE channel is ready
 *
 * @param channel the IDE channel
 * @return true if the given IDE channel is ready
 * @return false if the given IDE channel is not ready
 * @warning This function will loop many times for checking the status
 */
bool
ide_is_device_ready(struct ide_channel* channel);

/**
 * @brief Read a sector from the given IDE channel
 *
 * @param channel the IDE channel
 * @param buffer the buffer to read the sector into
 * @param sector the sector to read
 */
void
ide_read_sector(struct ide_channel* channel, void* buffer, size_t sector);

void
ide_write_sector(struct ide_channel* channel,
                 const void* buffer,
                 size_t sector);

DEFINE_IDE_CHANNEL_PORT(ide_channel_data, 0)
DEFINE_IDE_CHANNEL_PORT(ide_channel_error, 0x01)
DEFINE_IDE_CHANNEL_PORT(ide_channel_sec_cnt, 0x02)
DEFINE_IDE_CHANNEL_PORT(ide_channel_lba_lo, 0x03)
DEFINE_IDE_CHANNEL_PORT(ide_channel_lba_mid, 0x04)
DEFINE_IDE_CHANNEL_PORT(ide_channel_lba_hi, 0x05)
DEFINE_IDE_CHANNEL_PORT(ide_channel_dev, 0x06)
DEFINE_IDE_CHANNEL_PORT(ide_channel_status, 0x07)
DEFINE_IDE_CHANNEL_PORT(ide_channel_cmd, 0x07)
DEFINE_IDE_CHANNEL_PORT(ide_channel_alt_status, 0x206)
DEFINE_IDE_CHANNEL_PORT(ide_channel_dev_ctrl, 0x206)

#else

#define IDE_CMD_IDENTIFY 0xec
#define IDE_CMD_READ 0x20
#define IDE_CMD_WRITE 0x30

#define IDE_STATUS_BIT_BSY 0x80
#define IDE_STATUS_BIT_DRDY 0x40
#define IDE_STATUS_BIT_DRQ 0x08

#endif