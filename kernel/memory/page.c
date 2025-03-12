#include "kernel/memory/page.h"
#include "kernel/assert.h"
#include "kernel/common.h"
#include "kernel/config/memory.h"
#include "kernel/task/task.h"
#include "kernel/types.h"
#include "kernel/utils/asm.h"
#include "kernel/utils/bitmap.h"
#include "kernel/utils/string.h"

struct pmempool
{
  struct atomic_bitmap pmmap;
  u32 pstart;
  size_t size;
};

extern u32 _asm_mem_bytes;

static struct pmempool kmem_pool;
static struct pmempool umem_pool;
static struct vmemmap kvmmap;

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
  atomic_bitmap_init(&kmem_pool.pmmap, (void*)PAGE_BITMAP_VSTART, kbitmap_size);

  umem_pool.size = umem_pages * MEM_PAGE_SIZE;
  umem_pool.pstart = umem_start;
  atomic_bitmap_init(
    &umem_pool.pmmap, (void*)(PAGE_BITMAP_VSTART + kbitmap_size), ubitmap_size);

  kvmmap.vstart = PAGE_KERNEL_HEAP_VSTART;
  atomic_bitmap_init(&kvmmap.vmmap,
                     (void*)(PAGE_BITMAP_VSTART + kbitmap_size + ubitmap_size),
                     kbitmap_size);
}

static void*
__alloc_vmem_page(size_t size, bool kernel)
{
  AUTO vaddr_map = kernel ? &kvmmap : &task_current()->user_vaddr;

  ssize_t index = atomic_bitmap_alloc(&vaddr_map->vmmap, size);
  if (index == NPOS) {
    return NULL;
  }
  return (void*)(vaddr_map->vstart + index * MEM_PAGE_SIZE);
}

static void
__free_vmem_page(void* vaddr, size_t size, bool kernel)
{
  AUTO vaddr_map = kernel ? &kvmmap : &task_current()->user_vaddr;
  size_t index = ((u32)vaddr - vaddr_map->vstart) / MEM_PAGE_SIZE;
  for (size_t i = 0; i < size; i++) {
    atomic_bitmap_set(&vaddr_map->vmmap, index + i, false);
  }
}

static void*
__alloc_pmem_page(struct pmempool* pool)
{
  ssize_t index = atomic_bitmap_alloc(&pool->pmmap, true);
  if (index == NPOS) {
    return NULL;
  }
  return (void*)(pool->pstart + index * MEM_PAGE_SIZE);
}

static void
__free_pmem_page(struct pmempool* pool, void* paddr)
{
  size_t index = ((u32)paddr - pool->pstart) / MEM_PAGE_SIZE;
  atomic_bitmap_set(&pool->pmmap, index, false);
}

static void
__link_mem_page(void* paddr, void* vaddr)
{
  u32* pde = PAGE_GET_PDE(vaddr);
  u32* pte = PAGE_GET_PTE(vaddr);

  // KASSERT(
  //   !(*pte & PAGE_PTE_P_MASK), "duplicate page table allocation at %x",
  //   vaddr);

  if (*pde & PAGE_PDE_P_MASK) {
    if (!(*pte & PAGE_PTE_P_MASK)) {
      *pte = PAGE_PTE_DESC((u32)paddr, 1, 1, 1);
    }
  } else {
    void* new_page = __alloc_pmem_page(&kmem_pool);

    *pde = PAGE_PDE_DESC((u32)new_page, 1, 1, 1);

    kmemset((void*)((u32)pte & PAGE_SELECTOR_MASK), 0, MEM_PAGE_SIZE);
    *pte = PAGE_PTE_DESC((u32)paddr, 1, 1, 1);
  }
}

static void
__unlink_mem_page(void* vaddr)
{
  u32* pte = PAGE_GET_PTE(vaddr);
  *pte &= ~PAGE_PDE_P(1);
  invlpg(vaddr);
}

void*
alloc_page(size_t size, bool kernel)
{
  KASSERT(size <= MEM_POOL_MAXIMUM_PAGE_SINGLE_ALLOC,
          "requested too many pages, alloc %u pages, but only %u allowed",
          size,
          MEM_POOL_MAXIMUM_PAGE_SINGLE_ALLOC);

  AUTO pmem_pool = kernel ? &kmem_pool : &umem_pool;

  void* vm_addr = __alloc_vmem_page(size, kernel);
  if (vm_addr == NULL) {
    return NULL;
  }

  void* vmaddr_start = vm_addr;
  size_t alloc_size = size;
  while (size > 0) {
    void* pm_addr = __alloc_pmem_page(pmem_pool);
    if (pm_addr == NULL) {
      // * NOTE: code below is not tested
      KWARNING("failed to allocate physical memory, related vm_addr: %p",
               vm_addr);
      free_page(vmaddr_start, alloc_size - size);
      return NULL;
    }

    __link_mem_page(pm_addr, vm_addr);

    vm_addr += MEM_PAGE_SIZE;
    size--;
  }

  kmemset(vmaddr_start, 0, alloc_size * MEM_PAGE_SIZE);

  return vmaddr_start;
}

void*
link_page(void* vaddr, bool kernel)
{
  AUTO pmem_pool = kernel ? &kmem_pool : &umem_pool;
  AUTO vaddr_map = kernel ? &kvmmap : &task_current()->user_vaddr;

  size_t index = ((u32)vaddr - vaddr_map->vstart) / MEM_PAGE_SIZE;
  atomic_bitmap_set(&vaddr_map->vmmap, index, true);

  void* paddr = __alloc_pmem_page(pmem_pool);
  if (paddr == NULL) {
    // * NOTE: code below is not tested
    KWARNING("failed to allocate physical memory, related vm_addr: %p", vaddr);
    return NULL;
  }

  __link_mem_page(paddr, vaddr);

  return vaddr;
}

void
free_page(void* vaddr, size_t size)
{
  KASSERT(
    ((u32)vaddr & MEM_PAGE_SIZE) == 0, "vaddr %p is not page aligned", vaddr);

  void* paddr = PAGE_VADDR_TO_PADDR(vaddr);
  bool kernel = !((u32)paddr >= umem_pool.pstart);
  AUTO pmem_pool = kernel ? &kmem_pool : &umem_pool;

  KASSERT(
    ((u32)paddr & MEM_PAGE_SIZE) == 0, "paddr %p is not page aligned", paddr);
  KASSERT((u32)paddr >= KMEMMB(1) + KMEMKB(1),
          "cannot free paddr %p outside of memory pool",
          paddr);

  void* vaddr_start = vaddr;
  for (size_t i = 0; i < size; i++) {
    paddr = PAGE_VADDR_TO_PADDR(vaddr);
    KASSERT(
      ((u32)paddr & MEM_PAGE_SIZE) == 0, "paddr %p is not page aligned", paddr);
    // KASSERT((u32)paddr >= kmem_pool.pstart && (u32)paddr < umem_pool.pstart,
    //         "paddr %p is not kernel memory, but treated as such",
    //         paddr);

    __free_pmem_page(pmem_pool, paddr);
    __unlink_mem_page(vaddr);

    vaddr += MEM_PAGE_SIZE;
  }

  __free_vmem_page(vaddr_start, size, kernel);
}