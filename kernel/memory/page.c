#include "kernel/memory/page.h"
#include "common.h"
#include "kernel/assert.h"
#include "kernel/config/memory.h"
#include "kernel/types.h"
#include "kernel/utils/asm.h"
#include "kernel/utils/bitmap.h"
#include "kernel/utils/mm.h"

struct vmem_map
{
  struct bitmap vmmap;
  u32 vstart;
};

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

void
init_page(void)
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

static void
free_vmem_page(void* vaddr, size_t size)
{
  size_t index = ((u32)vaddr - kvmmap.vstart) / MEM_PAGE_SIZE;
  for (size_t i = 0; i < size; i++) {
    bitmap_set(&kvmmap.vmmap, index + i, false);
  }
}

static void*
alloc_pmem_page(struct pmempool* pool)
{
  ssize_t index = bitmap_alloc(&pool->pmmap, true);
  if (index == -1) {
    return NULL;
  }
  return (void*)(pool->pstart + index * MEM_PAGE_SIZE);
}

static void
free_pmem_page(struct pmempool* pool, void* paddr)
{
  size_t index = ((u32)paddr - pool->pstart) / MEM_PAGE_SIZE;
  bitmap_set(&pool->pmmap, index, false);
}

static void
link_mem_page(void* paddr, void* vaddr)
{
  u32* pde = PAGE_GET_PDE(vaddr);
  u32* pte = PAGE_GET_PTE(vaddr);

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

static void
unlink_mem_page(void* vaddr)
{
  u32* pte = PAGE_GET_PTE(vaddr);
  *pte &= ~PAGE_PDE_P(0);
  invlpg(vaddr);
}

void*
alloc_page(size_t size)
{
  KASSERT(size <= MEM_POOL_MAXIMUM_PAGE_SINGLE_ALLOC);

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

void
free_page(void* vaddr, size_t size)
{
  KASSERT(((u32)vaddr & MEM_PAGE_SIZE) == 0);
  void* paddr = PAGE_VADDR_TO_PADDR(vaddr);
  KASSERT(((u32)paddr & MEM_PAGE_SIZE) == 0 &&
          (u32)paddr >= KMEMMB(1) + KMEMKB(1));

  void* vaddr_start = vaddr;
  if ((u32)paddr >= umem_pool.pstart) {
    // TODO: free user memory
  } else {
    for (size_t i = 0; i < size; i++) {
      paddr = PAGE_VADDR_TO_PADDR(vaddr);
      KASSERT(((u32)paddr & MEM_PAGE_SIZE) == 0 &&
              (u32)paddr >= kmem_pool.pstart && (u32)paddr < umem_pool.pstart);

      free_pmem_page(&kmem_pool, paddr);
      unlink_mem_page(vaddr);

      vaddr += MEM_PAGE_SIZE;
    }

    free_vmem_page(vaddr_start, size);
  }
}