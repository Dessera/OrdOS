#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
};

struct partition
{
  size_t sec_start;
  size_t sec_cnt;

  struct partition* next;
  struct partition* prev;
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

void
partition_create_fs(struct partition* p)
{
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