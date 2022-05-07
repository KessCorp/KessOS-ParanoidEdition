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


static memdesc_t* ALLOC_BASE;
static uint64_t current_addr = 0xDEADBEEF;
static uint64_t segment_usable = 0x0;           // Amount of usable memory for this segment, MEASURED IN PAGES (0x1000 bytes per page).


__attribute__((aligned(0x1000))) static struct MappingTable {
    uint64_t entries[512];
} pml4;


// Locates biggest a free space a memory
// to hold allocation base (i.e first address to be allocated).
static void set_alloc_base(meminfo_t meminfo) {
    size_t n_entries = get_mmap_entries(meminfo);


    // Loop through each entry.
    for (int i = 0; i < n_entries; ++i) {
        memdesc_t* current = mmap_iterator_helper(i, meminfo);

        if (current->type == MMAP_USABLE_MEMORY) {
            ALLOC_BASE = current;
            current_addr = (uint64_t)current->physAddr;
            segment_usable = current->nPages;
            break;
        }
    }
}


static void* internel_alloc_page() {
    if (segment_usable <= 0) {
        log("ALLOC_PAGE_FAILED: OUT OF MEMORY!\n", S_PANIC);
        FULL_HALT;
    }

    uint64_t ret = current_addr;
    --segment_usable;
    current_addr += PAGE_SIZE;
    return (void*)ret;
}


void map_page(void* logical, unsigned int flags) {
    uint64_t addr = (uint64_t)logical;

    // Get all the indices for the mapping tables.
    uint32_t pml4_idx = (addr >> 39) & 0x1FF;               // Bits 47-39 are used to index PML4 to fetch PDPT.
    uint32_t pdpt_idx = (addr >> 30) & 0x1FF;               // Bits 38-30 are used to index PDPT to fetch page directory.
    uint32_t pd_idx = (addr >> 20) & (0x1FF | 1 << 9);      // Bits 29-20 are used to index page directory to fetch page table.
    uint32_t pt_idx = (addr >> 12) & 0x1FF;                 // Bits 20-12 are used to index page table to fetch the page.
    uint32_t p_offset = addr & 0x7FF;                          // Bits 0-11 contain the offset to the page.

    if (!(pml4.entries[pml4_idx] & PAGE_BIT_P_PRESENT)) {
        // No PDPT for this index, so we must allocate a page for PDPT.
        uint64_t pdpt = (uint64_t)internel_alloc_page();
        log("Yes!\n", S_INFO);
        memzero((void*)pdpt, PAGE_SIZE);
        log("Yes!\n", S_INFO);
        pml4.entries[pml4_idx] = (pdpt & PAGE_ADDR_MASK) | flags;               // Now make the PML4 entry contain the new PDPT.
        map_page((void*)pdpt, flags);
    }

    // Get our PDPT and we will fetch the page directory from it.
    struct MappingTable* pdpt = (struct MappingTable*)(pml4.entries[pml4_idx] & PAGE_ADDR_MASK);

    if (!(pdpt->entries[pdpt_idx] & PAGE_BIT_P_PRESENT)) {
        // No page directory for this index, so we will allocate a page for page directory.
        uint64_t pd = (uint64_t)internel_alloc_page();
        memzero((void*)pd, PAGE_SIZE);
        pdpt->entries[pdpt_idx] = (pd & PAGE_ADDR_MASK) | flags;
        map_page((void*)pd, flags);
    }

    struct MappingTable* pd = (struct MappingTable*)(pdpt->entries[pdpt_idx] & PAGE_ADDR_MASK);

    if (!(pd->entries[pd_idx] & PAGE_BIT_P_PRESENT)) {
        // No page table in this page director so we will allocate a page for it.
        uint64_t pt = (uint64_t)internel_alloc_page();
        memzero((void*)pt, PAGE_SIZE);
        pd->entries[pdpt_idx] = (pt & PAGE_ADDR_MASK) | flags;
        map_page((void*)pt, flags);
    }

    struct MappingTable* pt = (struct MappingTable*)(pd->entries[pdpt_idx] & PAGE_ADDR_MASK);

    if (!(pt->entries[pt_idx] & PAGE_BIT_P_PRESENT)) {
        // No page is mapped to this entry, so map it to our logical
        // address.
        pt->entries[pt_idx] = (addr & PAGE_ADDR_MASK) | flags;
    }
}


void vmm_init(meminfo_t meminfo) {
    log("Setting allocation base..\n", S_INFO);
    set_alloc_base(meminfo);
    log("Allocation base set at %x\n", S_INFO, current_addr);

    // If the address didn't change, something really funky happened.
    if (current_addr == 0xDEADBEEF) {
        log("ALLOCATE_BASE_SET_FAILED: NO USABLE MEMORY!\n", S_PANIC);
        FULL_HALT;
    }

    log("Zeroing PML4..\n", S_INFO);
    memzero(&pml4, PAGE_SIZE);
    int a;
    map_page(&a, PAGE_SIZE);
    log("Yes!\n", S_INFO);
}
