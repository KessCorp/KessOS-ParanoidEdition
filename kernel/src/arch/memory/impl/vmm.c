#include <arch/memory/vmm.h>
#include <stddef.h>
#include <stdint.h>
#include <debug/log.h>
#include <util/asm.h>
#include <util/mem.h>

// 2022 Ian Moffett <ian@kesscoin.com>

#define PAGE_BIT_P_PRESENT (1 << 0)
#define PAGE_BIT_RW_WRITABLE (1 << 1)
#define PAGE_BIT_US_USER (1 << 2)
#define PAGE_XD_NX (1 << 63)
#define PAGE_ADDR_MASK 0x000ffffffffff000
#define PAGE_BIT_A_ACCESSED (1 << 5)
#define PAGE_BIT_D_DIRTY (1 << 6)
#define GB 0x40000000UL
#define MB 0x100000 

static uint64_t segment_usable = 0x0;           // Amount of usable memory for this segment, MEASURED IN PAGES (0x1000 bytes per page).


__attribute__((aligned(0x1000))) static struct MappingTable {
    uint64_t entries[512];
} pml4;


memdesc_t biggestSegment;
uint64_t placement = 0xDEADBEEF;

static void set_alloc_base(meminfo_t meminfo) {
    uint64_t mmap_entries = get_mmap_entries(meminfo);

    biggestSegment.nPages = 0;

    for (uint64_t i = 0; i < mmap_entries; ++i) {
        memdesc_t* cur = mmap_iterator_helper(i, meminfo);
        if (cur->nPages > biggestSegment.nPages && cur->type == MMAP_USABLE_MEMORY) {
            biggestSegment = *cur;
        }
    }

    placement = (uint64_t)biggestSegment.physAddr;
    segment_usable = biggestSegment.nPages;
}


static void* internel_alloc_page() {
    if (segment_usable <= 0) {
        log("ALLOC_PAGE_FAILED: OUT OF MEMORY!\n", S_PANIC);
        FULL_HALT;
    }

    uint64_t ret = placement;
    --segment_usable;
    placement += PAGE_SIZE;
    return (void*)ret;
}


void map_page(void* logical, unsigned int flags) {
    uint64_t addr = (uint64_t)logical;
    flags |= PAGE_BIT_P_PRESENT;            // Make sure it is present.

    // Get indices from logical address.
    int pml4_idx = (addr >> 39) & 0x1FF;
    int pdpt_idx = (addr >> 30) & 0x1FF;
    int pd_idx = (addr >> 21) & 0x1FF;
    int pt_idx = (addr >> 12) & 0x1FF;

    if (!(pml4.entries[pml4_idx] & PAGE_BIT_P_PRESENT)) {
        // We did not define a PDPT for this entry in PML4.
        uint64_t pdpt = (uint64_t)internel_alloc_page();
        memzero((void*)pdpt, PAGE_SIZE);
        pml4.entries[pml4_idx] = (pdpt & PAGE_ADDR_MASK) | flags;
        map_page((void*)pdpt, flags);
    }

    struct MappingTable* pdpt = (struct MappingTable*)(pml4.entries[pml4_idx] & PAGE_ADDR_MASK);

    if (!(pdpt->entries[pdpt_idx] & PAGE_BIT_P_PRESENT)) {
        // We did not define PDT for this PDPT entry, so allocate a page for the PDT.
        uint64_t pdt = (uint64_t)internel_alloc_page();
        memzero((void*)pdt, PAGE_SIZE);
        pdpt->entries[pdpt_idx] = (pdt & PAGE_ADDR_MASK) | flags;
        map_page((void*)pdt, flags);
    }

    
    struct MappingTable* pdt = (struct MappingTable*)(pdpt->entries[pdpt_idx] & PAGE_ADDR_MASK);

    if (!(pdt->entries[pd_idx] & PAGE_BIT_P_PRESENT)) {
        // We did not define a PT for this PDT entry, so allocate a page for the PT.
        uint64_t pt = (uint64_t)internel_alloc_page();
        memzero((void*)pt, PAGE_SIZE);
        pdt->entries[pd_idx] = (pt & PAGE_ADDR_MASK) | flags;
        map_page((void*)pt, flags);
    }

    struct MappingTable* pt = (struct MappingTable*)(pdt->entries[pd_idx] & PAGE_ADDR_MASK);

    if (!(pt->entries[pt_idx] & PAGE_BIT_P_PRESENT)) {  
        pt->entries[pt_idx] = (addr & PAGE_ADDR_MASK) | flags;
    }
}


void vmm_init(meminfo_t meminfo) {
    log("Setting allocation base..\n", S_INFO);
    set_alloc_base(meminfo);

    // If the address didn't change, something really funky happened.
    if (placement == 0xDEADBEEF) {
        log("ALLOCATE_BASE_SET_FAILED: NO USABLE MEMORY!\n", S_PANIC);
        FULL_HALT;
    }

    log("Allocation base set at %x\n", S_INFO, placement);
 
    log("Zeroing PML4..\n", S_INFO);
    memzero(&pml4, PAGE_SIZE);

    log("Identity mapping some address space for KessKernel..\n", S_INFO);
    
    for (uint64_t addr = 0; addr < GB*2; addr += 4096) {
        map_page((void*)addr, PAGE_BIT_RW_WRITABLE | PAGE_BIT_P_PRESENT);
    }

    log("Finished mapping..\n", S_INFO);
    log("Loading PML4 into CR3 register..\n", S_INFO);
    load_pml4(&pml4);
}
