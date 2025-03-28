#include "kernel/device/disk/disk.h"
#include "kernel/boot.h"
#include "kernel/config/device.h"
#include "kernel/device/disk/ide.h"
#include "kernel/device/disk/partition.h"
#include "kernel/log.h"
#include "kernel/task/sync.h"
#include "kernel/utils/print.h"
#include "lib/string.h" // IWYU pragma: keep
#include "lib/types.h"

static struct disk __disks[IDE_CHANNEL_MAX_CNT * IDE_CHANNEL_DEVICE_MAX_CNT] = {
  0
};

static void
__disk_identify(struct disk* disk)
{
  AUTO channel = disk->channel;
  u8 buf[BOOT_SEC_SIZE] = { 0 };
  ide_select_device(channel, disk->dev_id);
  ide_send_cmd(channel, IDE_CMD_IDENTIFY);

  semaphore_down_nint(&channel->irq_sem);

  if (!ide_is_device_ready(disk->channel)) {
    KPANIC("unexpected device state when identifying disk %s", disk->name);
  }

  ide_read_sector(disk->channel, buf, 1);
  disk->sec_cnt = *(u32*)(buf + 120);
}

static void
__disk_init(struct disk* disk,
            char* name,
            struct ide_channel* channel,
            u8 dev_id)
{
  strncpy(disk->name, name, DEVICE_DISK_NAME_SIZE);
  disk->channel = channel;
  disk->dev_id = dev_id;

  __disk_identify(disk);
}

static void
__init_disk(void)
{
  AUTO disk_cnt = disk_get_cnt();
  char name[5] = { 0 };

  for (size_t i = 0; i < disk_cnt; i++) {
    if (i >= IDE_CHANNEL_MAX_CNT * IDE_CHANNEL_DEVICE_MAX_CNT) {
      break;
    }

    size_t channel_idx = i / 2;
    size_t dev_idx = i % 2;
    ksprint(name, "sd%c", 'a' + channel_idx + dev_idx);

    __disk_init(&__disks[i], name, ide_channel_get(channel_idx), dev_idx);
  }
}

void
init_disk(void)
{
  KINFO("initializing disk");

  init_ide();
  __init_disk();
  init_partition();
}

size_t
disk_get_index(struct disk* disk)
{
  size_t idx = disk - __disks;
  if (idx > disk_get_cnt()) {
    KPANIC("invalid disk index %u when getting disk index", idx);
  }
  return idx;
}

struct disk*
disk_get(size_t index)
{
  if (index >= disk_get_cnt()) {
    KPANIC("invalid disk index %u when getting disk", index);
  }
  return &__disks[index];
}

void
disk_read(struct disk* disk, u32 sec_start, u32 sec_cnt, void* buf)
{
  AUTO channel = disk->channel;

  mutex_lock(&channel->lock);

  ide_select_device(channel, disk->dev_id);

  size_t sec_done_cnt = 0;
  while (sec_done_cnt < sec_cnt) {
    size_t sec_to_rd =
      sec_done_cnt + 256 > sec_cnt ? sec_cnt - sec_done_cnt : 256;

    ide_select_sector(
      channel, disk->dev_id, sec_start + sec_done_cnt, sec_to_rd);

    ide_send_cmd(channel, IDE_CMD_READ);
    semaphore_down_nint(&channel->irq_sem);

    if (!ide_is_device_ready(channel)) {
      KPANIC("unexpected device state when reading disk %s", disk->name);
    }

    ide_read_sector(channel, buf + sec_done_cnt * BOOT_SEC_SIZE, sec_to_rd);

    sec_done_cnt += sec_to_rd;
  }

  mutex_unlock(&channel->lock);
}

void
disk_write(struct disk* disk, u32 sec_start, u32 sec_cnt, void* buf)
{
  AUTO channel = disk->channel;

  mutex_lock(&channel->lock);

  ide_select_device(channel, disk->dev_id);

  size_t sec_done_cnt = 0;
  while (sec_done_cnt < sec_cnt) {
    size_t sec_to_wr =
      sec_done_cnt + 256 > sec_cnt ? sec_cnt - sec_done_cnt : 256;

    ide_select_sector(
      channel, disk->dev_id, sec_start + sec_done_cnt, sec_to_wr);

    ide_send_cmd(channel, IDE_CMD_WRITE);

    if (!ide_is_device_ready(channel)) {
      KPANIC("unexpected device state when writing disk %s", disk->name);
    }

    ide_write_sector(channel, buf + sec_done_cnt * BOOT_SEC_SIZE, sec_to_wr);

    semaphore_down_nint(&channel->irq_sem); // wait for write to finish
    sec_done_cnt += sec_to_wr;
  }

  mutex_unlock(&channel->lock);
}
