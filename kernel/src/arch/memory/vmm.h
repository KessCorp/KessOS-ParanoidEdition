#ifndef VMM_H
#define VMM_H

#include <arch/memory/memory.h>

// 2022 Ian Moffett <ian@kesscoin.com>

void vmm_init(meminfo_t meminfo);
void map_page(void* logical, unsigned int flags);
uint8_t unmap_page(void* logical);
void load_pml4(void* pml4);

#endif
