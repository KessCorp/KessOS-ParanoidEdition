#include <util/mem.h>

// 2022 Ian Moffett <ian@kesscoin.com>


void memzero(void* addr, size_t n) {
    char* ch = (char*)addr;

    for (size_t i = 0; i < n; ++i) {
        ch[i] = 0x0;
    }
}
