#include "kernel/filesystem/filesystem.h"
#include "kernel/boot.h"
#include "kernel/config/filesystem.h"
#include "kernel/config/memory.h"
#include "kernel/device/disk/disk.h"
#include "kernel/device/disk/partition.h"
#include "kernel/log.h"
#include "kernel/memory/buddy/buddy.h"
#include "kernel/memory/buddy/page.h"
#include "kernel/memory/memory.h"
#include "kernel/utils/bitmap.h"
#include "lib/common.h"
#include "lib/list_head.h"
#include "lib/string.h" // IWYU pragma: keep
#include "lib/types.h"

void
init_fs(void)
{
  init_disk();

  if (partition_get_cnt() < 1) {
    KPANIC("no partitions found, cannot mount filesystem");
  } else if (!fs_mount(partition_get_by_index(0))) {
    KPANIC("failed to mount filesystem");
  }
}

bool
fs_mount(struct disk_partition* part)
{
  AUTO disk = part->disk;

  // fetch superblock
  void* KMALLOC_CLEANUP sb_buf = kmalloc(BOOT_SEC_SIZE);
  if (sb_buf == nullptr) {
    return false;
  }

  disk_read(disk, part->sec_start + 1, 1, sb_buf);
  if (((struct superblock*)sb_buf)->magic != FS_SUPERBLOCK_MAGIC) {
    return false;
  }

  part->sb = kmalloc(sizeof(struct superblock));
  if (part->sb == nullptr) {
    return false;
  }

  memcpy(part->sb, sb_buf, sizeof(struct superblock));

  // fetch block bitmap
  AUTO block_bitmap_page = buddy_alloc_page(
    MEM_ZONE_NORMAL,
    buddy_page_cnt_to_order(
      DIV_UP(part->sb->block_bitmap_sec_cnt * BOOT_SEC_SIZE, MEM_PAGE_SIZE)));
  if (block_bitmap_page == nullptr) {
    goto block_bitmap_alloc_fail;
  }
  void* block_bitmap = (void*)page_get_virt(block_bitmap_page);

  disk_read(disk,
            part->sb->block_bitmap_start,
            part->sb->block_bitmap_sec_cnt,
            block_bitmap);

  part->block_bitmap.data = block_bitmap;
  part->block_bitmap.size = part->sb->block_bitmap_sec_cnt * BOOT_SEC_SIZE;

  // fetch inode bitmap
  AUTO inode_bitmap_page = buddy_alloc_page(
    MEM_ZONE_NORMAL,
    buddy_page_cnt_to_order(
      DIV_UP(part->sb->inode_bitmap_sec_cnt * BOOT_SEC_SIZE, MEM_PAGE_SIZE)));
  if (inode_bitmap_page == nullptr) {
    goto inode_bitmap_alloc_fail;
  }
  void* inode_bitmap = (void*)page_get_virt(inode_bitmap_page);

  disk_read(disk,
            part->sb->inode_bitmap_start,
            part->sb->inode_bitmap_sec_cnt,
            inode_bitmap);

  part->inode_bitmap.data = inode_bitmap;
  part->inode_bitmap.size = part->sb->inode_bitmap_sec_cnt * BOOT_SEC_SIZE;

  // init open inodes list
  list_init(&part->open_inodes);

  return true;

inode_bitmap_alloc_fail:
  buddy_free_page(
    block_bitmap_page,
    buddy_page_cnt_to_order(
      DIV_UP(part->sb->block_bitmap_sec_cnt * BOOT_SEC_SIZE, MEM_PAGE_SIZE)));
block_bitmap_alloc_fail:
  kfree(part->sb);
  return false;
}

void
fs_umount(struct disk_partition* part)
{
  AUTO inode_page = page_get_by_virt((uintptr_t)part->inode_bitmap.data);
  AUTO block_bitmap_page = page_get_by_virt((uintptr_t)part->block_bitmap.data);

  buddy_free_page(
    inode_page,
    buddy_page_cnt_to_order(
      DIV_UP(part->sb->inode_bitmap_sec_cnt * BOOT_SEC_SIZE, MEM_PAGE_SIZE)));
  buddy_free_page(
    block_bitmap_page,
    buddy_page_cnt_to_order(
      DIV_UP(part->sb->block_bitmap_sec_cnt * BOOT_SEC_SIZE, MEM_PAGE_SIZE)));

  kfree(part->sb);
  part->sb = nullptr;
}