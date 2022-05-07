#include <util/mem.h>

// 2022 Ian Moffett <ian@kesscoin.com>


void memzero(void* addr, size_t n) {
    for (size_t i = 0; i < n; ++i)
        *(size_t*)(addr + i) = 0x0;
}
