#ifndef MSR_H
#define MSR_H

// 2022 Ian Moffett <ian@kesscoin.com>

#include <stdint.h>

static inline void rdmsr(uint32_t msr, uint32_t* lo, uint32_t* hi) {
   __asm__ __volatile__("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

static inline void wrmsr(uint32_t msr, uint32_t lo, uint32_t hi) {
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}


#endif
