#include "kernel/filesystem/inode.h"
#include "kernel/assert.h"
#include "kernel/boot.h"
#include "kernel/device/disk/disk.h"
#include "kernel/memory/memory.h"
#include "lib/list_head.h"
#include "lib/string.h" // IWYU pragma: keep

void
inode_init(struct inode* inode, u32 iid)
{
  memset(inode, 0, sizeof(struct inode));
  inode->dinode.iid = iid;
}

void
inode_locate(struct inode_position* pos, struct disk_partition* part, u32 iid)
{
  AUTO inode_table_offset = iid * sizeof(struct dinode);

  pos->sec = part->sb->inode_table_start + inode_table_offset / BOOT_SEC_SIZE;
  pos->offset = inode_table_offset % BOOT_SEC_SIZE;
}

bool
inode_sync(struct inode* inode, struct disk_partition* part)
{
  struct inode_position pos;
  inode_locate(&pos, part, inode->dinode.iid);
  KASSERT(pos.sec <= part->sec_start + part->sec_cnt,
          "invalid inode sec position");

  struct dinode dinode;
  memcpy(&dinode, &inode->dinode, sizeof(struct dinode));

  void* iobuf = kmalloc(BOOT_SEC_SIZE);
  if (iobuf == nullptr) {
    return false;
  }

  disk_read(part->disk, pos.sec, 1, iobuf);

  memcpy(iobuf + pos.offset, &dinode, sizeof(struct dinode));

  disk_write(part->disk, pos.sec, 1, iobuf);

  kfree(iobuf);
  return true;
}

struct inode*
inode_open(struct disk_partition* part, u32 iid)
{
  struct list_head* entry;
  LIST_FOR_EACH(entry, &part->open_inodes)
  {
    struct inode* inode = LIST_ENTRY(entry, struct inode, node);
    if (inode->dinode.iid == iid) {
      inode->ref_cnt++;
      return inode;
    }
  }

  struct inode_position pos;
  inode_locate(&pos, part, iid);

  struct inode* inode = kmalloc(sizeof(struct inode));
  if (inode == nullptr) {
    return nullptr;
  }

  void* iobuf = kmalloc(BOOT_SEC_SIZE);
  if (iobuf == nullptr) {
    kfree(inode);
    return nullptr;
  }

  disk_read(part->disk, pos.sec, 1, iobuf);
  memcpy(inode, iobuf + pos.offset, sizeof(struct dinode));

  list_add_tail(&inode->node, &part->open_inodes);
  inode->ref_cnt = 1;

  kfree(iobuf);
  return inode;
}

void
inode_close(struct inode* inode)
{
  if (--inode->ref_cnt == 0) {
    list_del(&inode->node);
    kfree(inode);
  }
}