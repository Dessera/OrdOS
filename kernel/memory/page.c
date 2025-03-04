#include "kernel/memory/page.h"
#include "common.h"
#include "kernel/assert.h"
#include "kernel/config/memory.h"
#include "kernel/types.h"
#include "kernel/utils/bitmap.h"
#include "kernel/utils/mm.h"
#include "kernel/utils/print.h"

#define CHK_ALLOC_SIZE(size) (size <= MEM_POOL_MAXIMUM_PAGE_SINGLE_ALLOC)

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

static void*
alloc_vmem_page(size_t size)
{
  ssize_t index = bitmap_alloc(&kvmmap.vmmap, size);
  if (index == -1) {
    return NULL;
  }
  return (void*)(kvmmap.vstart + index * MEM_PAGE_SIZE);
}

static void*
alloc_pmem_page(struct pmempool* pool)
{
  ssize_t index = bitmap_alloc(&pool->pmmap, 1);
  if (index == -1) {
    return NULL;
  }
  return (void*)(pool->pstart + index * MEM_PAGE_SIZE);
}

static void
link_mem_page(void* paddr, void* vaddr)
{
  u32* pde = PAGE_GET_PDE((u32)vaddr);
  u32* pte = PAGE_GET_PTE((u32)vaddr);

  KASSERT_MSG(!(*pte & PAGE_PTE_P_MASK), "Duplicate page table allocation");

  if (*pde & PAGE_PDE_P_MASK) {
    if (!(*pte & PAGE_PTE_P_MASK)) {
      *pte = PAGE_PTE_DESC((u32)paddr, 1, 1, 1);
    }
  } else {
    void* new_page = alloc_pmem_page(&kmem_pool);

    *pde = PAGE_PDE_DESC((u32)new_page, 1, 1, 1);

    kmemset(pte, 0, MEM_PAGE_SIZE);
    *pte = PAGE_PTE_DESC((u32)paddr, 1, 1, 1);
  }
}

void
init_mm(void)
{
  kputs("Initializing memory management...\n");

  init_mempool();
}

void*
alloc_page(size_t size)
{
  KASSERT(CHK_ALLOC_SIZE(size));

  void* vm_addr = alloc_vmem_page(size);
  if (vm_addr == NULL) {
    return NULL;
  }

  void* vmaddr_start = vm_addr;
  while (size > 0) {
    void* pm_addr = alloc_pmem_page(&kmem_pool);
    if (pm_addr == NULL) {
      // TODO: revert all allocations
      return NULL;
    }

    link_mem_page(pm_addr, vm_addr);

    vm_addr += MEM_PAGE_SIZE;
    size--;
  }

  return vmaddr_start;
}