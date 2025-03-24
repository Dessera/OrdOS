#include "kernel/device/disk/ide.h"
#include "kernel/assert.h"
#include "kernel/boot.h"
#include "kernel/interrupt/interrupt.h"
#include "kernel/task/sync.h"
#include "kernel/task/task.h"
#include "kernel/utils/string.h"
#include "lib/asm.h"
#include "lib/types.h"

static struct ide_channel __channels[IDE_CHANNEL_MAX_CNT] = { 0 };

static void
__ide_irq_handler(u32 irq)
{
  KASSERT(irq == 0x2e || irq == 0x2f, "invalid disk irq: %x", irq);
  AUTO channel = &__channels[irq - 0x2e];

  if (!channel->irq_ready) {
    channel->irq_ready = true;
    semaphore_up(&channel->irq_sem);

    inb(ide_channel_status(channel));
  }
}

static void
__ide_channel_init(struct ide_channel* channel,
                   char* name,
                   u16 base_port,
                   u32 irq)
{
  kstrcpy(channel->name, name);
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
ide_send_cmd(struct ide_channel* channel, enum ide_cmd cmd)
{
  channel->irq_ready = false;
  outb(ide_channel_cmd(channel), cmd);
  semaphore_down(&channel->irq_sem);
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