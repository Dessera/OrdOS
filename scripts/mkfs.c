#include "filesystem.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOOT_SEC_SIZE 512

struct superblock
{
  uint32_t magic;
  uint32_t sec_cnt;
  uint32_t inode_cnt;
  uint32_t sec_start;

  uint32_t block_bitmap_start;
  uint32_t block_bitmap_sec_cnt;

  uint32_t inode_bitmap_start;
  uint32_t inode_bitmap_sec_cnt;

  uint32_t inode_table_start;
  uint32_t inode_table_sec_cnt;

  uint32_t data_start;
  uint32_t root_inode_idx;
  uint32_t direntry_size;

  uint8_t other[460];
} __attribute__((packed));

struct inode
{
  uint32_t iid;
  uint32_t size;
  uint32_t ref_cnt;
  uint32_t direct[12];
  uint32_t indirect;

  // originally list_head but we don't have that
  uint64_t node_padding;
};

struct direntry
{
  uint32_t inode_idx;
  char name[FS_FILENAME_SIZE];
  int32_t type_padding;
};

struct partition
{
  size_t sec_start;
  size_t sec_cnt;

  struct partition* next;
  struct partition* prev;

  FILE* fp;
};

void
partition_list_add(struct partition* head, struct partition* p)
{
  p->next = head;
  p->prev = head->prev;
  head->prev->next = p;
  head->prev = p;
}

void
partition_list_remove(struct partition* head, struct partition* p)
{
  p->prev->next = p->next;
  p->next->prev = p->prev;
}

void
create_partition_list(struct partition* head)
{
  head->sec_start = 0;
  head->sec_cnt = 0;
  head->next = head;
  head->prev = head;
}

void
destroy_partition_list(struct partition* p)
{
  struct partition* head = p;
  struct partition* entry = p->next;

  while (entry != head) {
    struct partition* tmp = entry;
    entry = entry->next;
    free(tmp);
  }

  free(head);
}

struct partition_table_entry
{
  uint8_t bootable;
  uint8_t start_head;
  uint8_t start_sec;
  uint8_t start_chs;
  uint8_t fs_type;
  uint8_t end_head;
  uint8_t end_sec;
  uint8_t end_chs;
  uint32_t start_lba;
  uint32_t sec_cnt;
} __attribute__((packed));

struct boot_sector
{
  uint8_t other[446];
  struct partition_table_entry partition_table[4];
  uint16_t signature;
} __attribute__((packed));

bool
image_read_sec(FILE* fp, void* buf, size_t sec, size_t cnt)
{
  fseek(fp, sec * 512, SEEK_SET);
  size_t num_read = fread(buf, 512, cnt, fp);
  if (num_read != cnt) {
    perror("read sector");
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
    perror("write sector");
    return false;
  }

  return true;
}

void
__list_partitions(FILE* fp, struct partition* head, size_t start, size_t base)
{
  struct boot_sector* bs = malloc(sizeof(struct boot_sector));
  if (!bs) {
    perror("malloc boot sector");
    return;
  }

  if (!image_read_sec(fp, bs, start, 1)) {
    goto lsp_done;
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
      struct partition* new = malloc(sizeof(struct partition));
      if (!new) {
        perror("malloc partition");
        goto lsp_done;
      }

      new->sec_start = p[i].start_lba + start;
      new->sec_cnt = p[i].sec_cnt;
      new->fp = fp;
      partition_list_add(head, new);
    }
  }

lsp_done:
  free(bs);
  return;
}

struct partition*
list_partitions(FILE* fp)
{
  struct partition* head = malloc(sizeof(struct partition));
  if (!head) {
    perror("malloc partition list");
    return nullptr;
  }
  create_partition_list(head);

  __list_partitions(fp, head, 0, 0);
  return head;
}

void
partition_create_fs(struct partition* p)
{
  uint32_t inode_bmap_size = DIV_UP(FS_MAX_FILES, BOOT_SEC_SIZE * 8);
  uint32_t inode_table_size =
    DIV_UP(FS_MAX_FILES * (sizeof(struct inode)), BOOT_SEC_SIZE);
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

  if (!image_write_sec(p->fp, &sb, p->sec_start + 2, 1)) {
    perror("write superblock");
    exit(EXIT_FAILURE);
  }

  // init block bitmap
  uint8_t* block_bitmap = malloc(sb.block_bitmap_sec_cnt * BOOT_SEC_SIZE);
  if (!block_bitmap) {
    perror("malloc block bitmap");
    exit(EXIT_FAILURE);
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
    perror("write block bitmap");
    exit(EXIT_FAILURE);
  }
  free(block_bitmap);

  // init inode bitmap
  uint8_t* inode_bitmap = malloc(sb.inode_bitmap_sec_cnt * BOOT_SEC_SIZE);
  if (!inode_bitmap) {
    perror("malloc inode bitmap");
    exit(EXIT_FAILURE);
  }

  inode_bitmap[0] |= 0x01;

  if (!image_write_sec(
        p->fp, inode_bitmap, sb.inode_bitmap_start, sb.inode_bitmap_sec_cnt)) {
    perror("write inode bitmap");
    exit(EXIT_FAILURE);
  }
  free(inode_bitmap);

  // init inode table
  struct inode* inode_table = malloc(sb.inode_table_sec_cnt * BOOT_SEC_SIZE);
  if (!inode_table) {
    perror("malloc inode table");
    exit(EXIT_FAILURE);
  }

  inode_table[0].size = sb.direntry_size * 2;
  inode_table[0].iid = 0;
  inode_table[0].direct[0] = sb.data_start;

  if (!image_write_sec(
        p->fp, inode_table, sb.inode_table_start, sb.inode_table_sec_cnt)) {
    perror("write inode table");
    exit(EXIT_FAILURE);
  }
  free(inode_table);

  struct direntry* root_dir = malloc(BOOT_SEC_SIZE);
  if (!root_dir) {
    perror("malloc root dir");
    exit(EXIT_FAILURE);
  }

  root_dir[0].inode_idx = 0;
  strcpy(root_dir[0].name, ".");
  root_dir[0].type_padding = 1;

  root_dir[1].inode_idx = 0;
  strcpy(root_dir[1].name, "..");
  root_dir[1].type_padding = 1;

  if (!image_write_sec(p->fp, root_dir, sb.data_start, 1)) {
    perror("write root dir");
    exit(EXIT_FAILURE);
  }
  free(root_dir);

  printf("fs info:\n");
  printf("  block bitmap start: %u\n", sb.block_bitmap_start);
  printf("  block bitmap size: %u\n", sb.block_bitmap_sec_cnt);
  printf("  inode bitmap start: %u\n", sb.inode_bitmap_start);
  printf("  inode bitmap size: %u\n", sb.inode_bitmap_sec_cnt);
  printf("  inode table start: %u\n", sb.inode_table_start);
  printf("  inode table size: %u\n", sb.inode_table_sec_cnt);
  printf("  data start: %u\n", sb.data_start);
  printf("  root inode idx: %u\n", sb.root_inode_idx);
  printf("  direntry size: %u\n", sb.direntry_size);
}

int
main(int argc, char* argv[])
{
  if (argc < 2) {
    printf("Usage: %s <disk_img>\n", argv[0]);
    return 1;
  }

  // for now the program only creates a file system with nothing in it
  FILE* fp = fopen(argv[1], "rb+");
  if (!fp) {
    perror("open disk image");
    return 1;
  }

  struct partition* ps = list_partitions(fp);

  struct partition* p = ps->next;
  while (p != ps) {
    partition_create_fs(p);
    p = p->next;
  }

  destroy_partition_list(ps);

  fclose(fp);

  return 0;
}