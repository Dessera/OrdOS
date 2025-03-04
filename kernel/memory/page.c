#include "kernel/memory/page.h"
#include "common.h"
#include "kernel/config/memory.h"
#include "kernel/utils/bitmap.h"
#include "kernel/utils/print.h"

struct pmempool
{
  struct bitmap pmmap;
  u32 pstart;
  size_t size;
};

extern u32 _asm_mem_bytes;

static struct pmempool kmem_pool;
static struct pmempool umem_pool;
static struct vmem_map kvmmap;

static void
init_mempool(void)
{
  size_t mem_used = MEM_PAGE_SIZE * PAGE_INITIAL_ENTRIES +
                    KMEMMB(1); // 256 pages for kernel + 1mb low mem
  size_t free_mem = _asm_mem_bytes - mem_used;
  size_t free_pages = free_mem / MEM_PAGE_SIZE;

  size_t kmem_pages = free_pages / MEM_POOL_KERNEL_FACTOR;
  size_t umem_pages = free_pages - kmem_pages;

  size_t kbitmap_size = kmem_pages / 8;
  size_t ubitmap_size = umem_pages / 8;

  u32 kmem_start = mem_used;
  u32 umem_start = kmem_start + kmem_pages * MEM_PAGE_SIZE;

  kmem_pool.size = kmem_pages * MEM_PAGE_SIZE;
  kmem_pool.pstart = kmem_start;
  bitmap_init(&kmem_pool.pmmap, (void*)PAGE_BITMAP_VSTART, kbitmap_size);

  umem_pool.size = umem_pages * MEM_PAGE_SIZE;
  umem_pool.pstart = umem_start;
  bitmap_init(
    &umem_pool.pmmap, (void*)(PAGE_BITMAP_VSTART + kbitmap_size), ubitmap_size);

  kvmmap.vstart = PAGE_KERNEL_HEAP_VSTART;
  bitmap_init(&kvmmap.vmmap,
              (void*)(PAGE_BITMAP_VSTART + kbitmap_size + ubitmap_size),
              kbitmap_size);
}

void
init_mm(void)
{
  kputs("Initializing memory management...\n");

  init_mempool();
}