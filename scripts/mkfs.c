#include "mkfs.h"

#include "kernel/boot.h"
#include "kernel/device/disk/partition.h"
#include "kernel/filesystem/filesystem.h"
#include "kernel/filesystem/inode.h"
#include "lib/types.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
fcleanup(FILE** fp)
{
  fclose(*fp);
}

static void
partition_cleanup(struct image_partition** p)
{
  destroy_partition_list(*p);
}

static void
mem_cleanup(void* pptr)
{
  void** pp = (void**)pptr;
  free(*pp);
}

#define MALLOC_CLEANUP CLEAN_UP(mem_cleanup)

int
main(int argc, char* argv[])
{
  if (argc < 2) {
    printf("Usage: %s <disk_img>\n", argv[0]);
    return 1;
  }

  // for now the program only creates a file system with nothing in it
  FILE* CLEAN_UP(fcleanup) fp = fopen(argv[1], "rb+");
  if (!fp) {
    perror("fopen");
    return EXIT_FAILURE;
  }

  struct image_partition* CLEAN_UP(partition_cleanup) ps = list_partitions(fp);
  if (ps == nullptr) {
    perror("list_partitions");
    return EXIT_FAILURE;
  }

  struct image_partition* p = ps->next;
  while (p != ps) {
    if (partition_check_fs(p)) {
      fprintf(
        stderr, "partition %zu already has a file system\n", p->sec_start);

    } else if (!partition_create_fs(p)) {
      fprintf(stderr,
              "failed to to create file system on partition %zu:",
              p->sec_start);
      perror("  partition_create_fs");
    } else {
      fprintf(stdout, "created file system on partition %zu\n", p->sec_start);
    }
    p = p->next;
  }
}

void
partition_list_add(struct image_partition* head, struct image_partition* p)
{
  p->next = head;
  p->prev = head->prev;
  head->prev->next = p;
  head->prev = p;
}

static void
__list_partitions(FILE* fp,
                  struct image_partition* head,
                  size_t start,
                  size_t base)
{
  struct boot_sector* MALLOC_CLEANUP bs = malloc(sizeof(struct boot_sector));
  if (bs == nullptr) {
    return;
  }

  if (!image_read_sec(fp, bs, start, 1)) {
    return;
  }

  struct partition_table_entry* p = bs->partition_table;

  for (size_t i = 0; i < 4; i++) {
    if (p[i].fs_type == 0x05) {
      if (base == 0) {
        __list_partitions(fp, head, p[i].start_lba, p[i].start_lba);
      } else {
        __list_partitions(fp, head, p[i].start_lba + base, base);
      }
    } else if (p[i].fs_type != 0) {
      struct image_partition* new = malloc(sizeof(struct image_partition));
      if (new == nullptr) {
        return;
      }

      new->sec_start = p[i].start_lba + start;
      new->sec_cnt = p[i].sec_cnt;
      new->fp = fp;
      partition_list_add(head, new);
    }
  }
}

struct image_partition*
list_partitions(FILE* fp)
{
  struct image_partition* head = malloc(sizeof(struct image_partition));
  if (!head) {
    return nullptr;
  }

  head->sec_start = 0;
  head->sec_cnt = 0;
  head->next = head;
  head->prev = head;

  __list_partitions(fp, head, 0, 0);
  return head;
}

void
destroy_partition_list(struct image_partition* p)
{
  if (p == nullptr) {
    return;
  }

  struct image_partition* head = p;
  struct image_partition* entry = p->next;

  while (entry != head) {
    struct image_partition* tmp = entry;
    entry = entry->next;
    free(tmp);
  }

  free(head);
}

bool
image_read_sec(FILE* fp, void* buf, size_t sec, size_t cnt)
{
  fseek(fp, sec * 512, SEEK_SET);
  size_t num_read = fread(buf, 512, cnt, fp);
  if (num_read != cnt) {
    return false;
  }

  return true;
}

bool
image_write_sec(FILE* fp, void* buf, size_t sec, size_t cnt)
{
  fseek(fp, sec * 512, SEEK_SET);
  size_t num_written = fwrite(buf, 512, cnt, fp);
  if (num_written != cnt) {
    return false;
  }

  return true;
}

bool
partition_create_fs(struct image_partition* p)
{
  uint32_t inode_bmap_size = DIV_UP(FS_MAX_FILES, BOOT_SEC_SIZE * 8);
  uint32_t inode_table_size =
    DIV_UP(FS_MAX_FILES * (sizeof(struct dinode)), BOOT_SEC_SIZE);
  uint32_t used_size = inode_bmap_size + inode_table_size + 2;
  uint32_t free_size = p->sec_cnt - used_size;

  uint32_t block_bmap_size = DIV_UP(free_size, BOOT_SEC_SIZE * 8);
  free_size -= block_bmap_size;
  block_bmap_size = DIV_UP(free_size, BOOT_SEC_SIZE * 8);

  struct superblock sb = { 0 };
  sb.magic = FS_SUPERBLOCK_MAGIC;
  sb.sec_cnt = p->sec_cnt;
  sb.inode_cnt = FS_MAX_FILES;
  sb.sec_start = p->sec_start;

  sb.block_bitmap_start = sb.sec_start + 2;
  sb.block_bitmap_sec_cnt = block_bmap_size;

  sb.inode_bitmap_start = sb.block_bitmap_start + sb.block_bitmap_sec_cnt;
  sb.inode_bitmap_sec_cnt = inode_bmap_size;

  sb.inode_table_start = sb.inode_bitmap_start + sb.inode_bitmap_sec_cnt;
  sb.inode_table_sec_cnt = inode_table_size;

  sb.data_start = sb.inode_table_start + sb.inode_table_sec_cnt;
  sb.root_inode_idx = 0;
  sb.direntry_size = sizeof(struct direntry);

  if (!image_write_sec(p->fp, &sb, p->sec_start + 1, 1)) {
    return false;
  }

  // init block bitmap
  uint8_t* MALLOC_CLEANUP block_bitmap =
    malloc(sb.block_bitmap_sec_cnt * BOOT_SEC_SIZE);
  if (!block_bitmap) {
    return false;
  }

  block_bitmap[0] |= 0x01;

  uint32_t block_bitmap_last_idx = free_size / 8;
  uint8_t block_bitmap_last_bit = free_size % 8;
  uint32_t last_size = BOOT_SEC_SIZE - (block_bitmap_last_idx % BOOT_SEC_SIZE);

  memset(&block_bitmap[block_bitmap_last_idx], 0xff, last_size);

  for (size_t i = 0; i <= block_bitmap_last_bit; i++) {
    block_bitmap[block_bitmap_last_idx] &= ~(1 << i);
  }

  if (!image_write_sec(
        p->fp, block_bitmap, sb.block_bitmap_start, sb.block_bitmap_sec_cnt)) {
    return false;
  }

  // init inode bitmap
  uint8_t* MALLOC_CLEANUP inode_bitmap =
    malloc(sb.inode_bitmap_sec_cnt * BOOT_SEC_SIZE);
  if (!inode_bitmap) {
    return false;
  }

  inode_bitmap[0] |= 0x01;

  if (!image_write_sec(
        p->fp, inode_bitmap, sb.inode_bitmap_start, sb.inode_bitmap_sec_cnt)) {
    return false;
  }

  // init inode table
  struct dinode* MALLOC_CLEANUP inode_table =
    malloc(sb.inode_table_sec_cnt * BOOT_SEC_SIZE);
  if (!inode_table) {
    return false;
  }

  inode_table[0].size = sb.direntry_size * 2;
  inode_table[0].iid = 0;
  inode_table[0].direct[0] = sb.data_start;

  if (!image_write_sec(
        p->fp, inode_table, sb.inode_table_start, sb.inode_table_sec_cnt)) {
    return false;
  }

  struct direntry* MALLOC_CLEANUP root_dir = malloc(BOOT_SEC_SIZE);
  if (!root_dir) {
    return false;
  }

  root_dir[0].iid = 0;
  strcpy(root_dir[0].name, ".");
  root_dir[0].type = FT_DIRECTORY;

  root_dir[1].iid = 0;
  strcpy(root_dir[1].name, "..");
  root_dir[1].type = FT_DIRECTORY;

  if (!image_write_sec(p->fp, root_dir, sb.data_start, 1)) {
    return false;
  }

  return true;
}

bool
partition_check_fs(struct image_partition* p)
{
  void* MALLOC_CLEANUP sb_buf = malloc(BOOT_SEC_SIZE);

  if (!image_read_sec(p->fp, sb_buf, p->sec_start + 1, 1)) {
    return false;
  }

  struct superblock* sb = (struct superblock*)sb_buf;

  if (sb->magic != FS_SUPERBLOCK_MAGIC) {
    return false;
  }

  return true;
}
