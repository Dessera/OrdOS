#include "kernel/device/disk/ide.h"
#include "kernel/assert.h"
#include "kernel/boot.h"
#include "kernel/config/device.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/task/sync.h"
#include "kernel/task/task.h"
#include "lib/asm.h"
#include "lib/string.h"
#include "lib/types.h"

static struct ide_channel __channels[IDE_CHANNEL_MAX_CNT] = { 0 };

static void
__ide_irq_handler(u32 irq)
{
  KASSERT(irq == 0x2e || irq == 0x2f, "invalid disk irq: %x", irq);
  AUTO channel = &__channels[irq - 0x2e];

  if (!channel->irq_ready) {
    channel->irq_ready = true;
    semaphore_up_nint(&channel->irq_sem);

    inb(ide_channel_status(channel));
  }
}

static void
__ide_channel_init(struct ide_channel* channel,
                   char* name,
                   u16 base_port,
                   u32 irq)
{
  strncpy(channel->name, name, DEVICE_IDE_CHANNEL_NAME_SIZE);
  channel->base_port = base_port;
  channel->irq = irq;
  channel->irq_ready = true;

  mutex_lock_init(&channel->lock);
  semaphore_init(&channel->irq_sem, 0);

  intr_register_handler(irq, __ide_irq_handler);
}

void
init_ide(void)
{
  __ide_channel_init(&__channels[0], "ide0", IDE0_BASE_PORT, 0x2e);
  __ide_channel_init(&__channels[1], "ide1", IDE1_BASE_PORT, 0x2f);
}

FORCE_INLINE size_t
ide_channel_get_index(struct ide_channel* channel)
{
  size_t index = channel - __channels;
  KASSERT(index < IDE_CHANNEL_MAX_CNT, "invalid ide channel index: %u", index);
  return index;
}

FORCE_INLINE struct ide_channel*
ide_channel_get(size_t index)
{
  KASSERT(index < IDE_CHANNEL_MAX_CNT, "invalid ide channel index: %u", index);
  return &__channels[index];
}

void
ide_select_device(struct ide_channel* channel, size_t dev_index)
{
  u8 payload = IDE_DEV_BIT_MBS | IDE_DEV_BIT_LBA;
  if (dev_index != 0) {
    payload |= IDE_DEV_BIT_DEV;
  }

  outb(ide_channel_dev(channel), payload);
}

void
ide_select_sector(struct ide_channel* channel,
                  size_t dev_index,
                  size_t sec_start,
                  size_t sec_cnt)
{
  outb(ide_channel_sec_cnt(channel), sec_cnt);

  outb(ide_channel_lba_lo(channel), sec_start & 0xff);
  outb(ide_channel_lba_mid(channel), (sec_start >> 8) & 0xff);
  outb(ide_channel_lba_hi(channel), (sec_start >> 16) & 0xff);

  u8 payload = IDE_DEV_BIT_LBA | IDE_DEV_BIT_MBS;
  if (dev_index != 0) {
    payload |= IDE_DEV_BIT_DEV;
  }

  outb(ide_channel_dev(channel), payload | (sec_start >> 24));
}

void
ide_send_cmd(struct ide_channel* channel, enum ide_cmd cmd)
{
  channel->irq_ready = false;
  outb(ide_channel_cmd(channel), cmd);
}

bool
ide_is_device_ready(struct ide_channel* channel)
{
  size_t repeat_cnt = 0;
  while (repeat_cnt++ < 1000) {
    if ((inb(ide_channel_status(channel)) & IDE_STATUS_BIT_BSY) == 0) {
      return inb(ide_channel_status(channel)) & IDE_STATUS_BIT_DRQ;
    } else {
      task_sleep_millses(10);
    }
  }

  return false;
}

void
ide_read_sector(struct ide_channel* channel, void* buffer, size_t sector)
{
  insw(ide_channel_data(channel), buffer, sector * BOOT_SEC_SIZE / 2);
}

void
ide_write_sector(struct ide_channel* channel, const void* buffer, size_t sector)
{
  outsw(ide_channel_data(channel), buffer, sector * BOOT_SEC_SIZE / 2);
}