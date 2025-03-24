#include "kernel/device/disk.h"
#include "kernel/boot.h"
#include "kernel/device/disk/disk.h"
#include "kernel/device/disk/ide.h"
#include "kernel/log.h"
#include "kernel/utils/string.h"
#include "lib/types.h"

// static struct disk* __disks[IDE_CHANNEL_MAX_CNT * 2] = { 0 };

static void
__disk_init(struct disk* disk,
            char* name,
            struct ide_channel* channel,
            u8 dev_id)
{
  kstrcpy(disk->name, name);
  disk->channel = channel;
  disk->dev_id = dev_id;

  disk_identify(disk);
}

void
init_disk(void)
{
  init_ide();

  __disk_init(&ide_channel_get(0)->devices[0], "hda", ide_channel_get(0), 0);
  // __disk_init(&ide_channel_get(0)->devices[1], "hdb", ide_channel_get(0), 1);
  // __disk_init(&ide_channel_get(1)->devices[0], "hdc", ide_channel_get(1), 0);
  // __disk_init(&ide_channel_get(1)->devices[1], "hdd", ide_channel_get(1), 1);

  KINFO("disk init done");
}

void
disk_identify(struct disk* disk)
{
  char buf[BOOT_SEC_SIZE] = { 0 };
  ide_select_device(disk->channel, disk->dev_id);
  ide_send_cmd(disk->channel, IDE_CMD_IDENTIFY);

  if (!ide_is_device_ready(disk->channel)) {
    KPANIC("unexpected device state when identifying disk %s", disk->name);
  }

  ide_read_sector(disk->channel, buf, 1);
}