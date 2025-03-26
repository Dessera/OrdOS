#include "kernel/device/disk/partition.h"
#include "kernel/device/disk/disk.h"
#include "kernel/log.h"
#include "kernel/memory/memory.h"
#include "kernel/utils/list_head.h"
#include "lib/types.h"

struct list_head __partition_list;

struct partition_table_entry
{
  u8 bootable;
  u8 start_head;
  u8 start_sec;
  u8 start_chs;
  u8 fs_type;
  u8 end_head;
  u8 end_sec;
  u8 end_chs;
  u32 start_lba;
  u32 sec_cnt;
} __attribute__((packed));

struct boot_sector
{
  u8 other[446];
  struct partition_table_entry partition_table[4];
  u16 signature;
} __attribute__((packed));

void
init_partition(void)
{
  list_init(&__partition_list);
}

static void
__disk_partition_scan(struct disk* disk, size_t sec_start, size_t sec_base)
{
  struct boot_sector* bs = kmalloc(sizeof(struct boot_sector));
  if (bs == NULL) {
    KWARNING("no memory for boot sector when scanning partitions");
    return;
  }

  disk_read(disk, sec_start, 1, bs);

  AUTO partition_table = bs->partition_table;

  for (size_t part_idx = 0; part_idx < 4; part_idx++) {
    if (partition_table[part_idx].fs_type == 0x05) {
      if (sec_base == 0) {
        __disk_partition_scan(disk,
                              partition_table[part_idx].start_lba,
                              partition_table[part_idx].start_lba);
      } else {
        __disk_partition_scan(
          disk, partition_table[part_idx].start_lba + sec_base, sec_base);
      }
    } else if (partition_table[part_idx].fs_type != 0x00) {
      KINFO("found partition at %d, sec %d",
            partition_table[part_idx].start_lba + sec_start,
            partition_table[part_idx].sec_cnt);
    }
  }

  kfree(bs);
}

void
disk_partition_scan(struct disk* disk)
{
  __disk_partition_scan(disk, 0, 0);
}