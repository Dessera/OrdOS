#include "kernel/device/disk.h"
#include "kernel/boot.h"
#include "kernel/device/disk/disk.h"
#include "kernel/device/disk/ide.h"
#include "kernel/log.h"
#include "kernel/utils/print.h"
#include "kernel/utils/string.h"
#include "lib/types.h"

// static struct disk* __disks[IDE_CHANNEL_MAX_CNT * 2] = { 0 };

// static void
// __adjust_disk_buffer(u8* src, size_t start, size_t size)
// {
//   for (size_t i = 0; i < size; i += 2) {
//     if (i + 1 >= size) {
//       break;
//     }

//     u8 tmp = src[start + i];
//     src[start + i] = src[start + i + 1];
//     src[start + i + 1] = tmp;
//   }

//   src[start + size - 1] = 0;
// }

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

  for (int i = 0; i < disk_get_cnt(); i++) {
    if (i >= IDE_CHANNEL_MAX_CNT * 2) {
      break;
    }

    size_t channel_idx = i / 2;
    size_t dev_idx = i % 2;
    char name[5] = { 0 };
    ksprint(name, "hd%c", 'a' + channel_idx + dev_idx);

    __disk_init(&ide_channel_get(channel_idx)->devices[dev_idx],
                name,
                ide_channel_get(channel_idx),
                dev_idx);
  }

  KINFO("disk init done");
}

void
disk_identify(struct disk* disk)
{
  u8 buf[BOOT_SEC_SIZE] = { 0 };
  ide_select_device(disk->channel, disk->dev_id);
  ide_send_cmd(disk->channel, IDE_CMD_IDENTIFY);

  if (!ide_is_device_ready(disk->channel)) {
    KPANIC("unexpected device state when identifying disk %s", disk->name);
  }

  ide_read_sector(disk->channel, buf, 1);
  disk->sec_cnt = *(u32*)(buf + 120);
}