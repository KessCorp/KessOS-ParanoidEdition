#include <interrupts/IDT.h>

// 2022 Ian Moffett <ian@kesscoin.com>

// Vectors are just an entry.
#define MAX_IDT_VECTORS 256

static struct IDTVector idt[MAX_IDT_VECTORS];
static struct IDTPtr idt_ptr;

/*
 *  <==== Description 1 ====>
 *
 *
 *  Each hex value is 4 bits, thus 2 will be 8 and 4 will be 16.
 *  We are ANDing addr by 0xFFFF which will only get the first 16 bits.
 *  
 *  <==== Description 1 ====>
 *
 *  We are shifting addr by 16 thus knocking out the first 16 bits, then 
 *  we AND it by 0xFFFF thus getting only 16 bits which means we got the middle 16 bits.
 *
 *  <==== Description 1 ====>
 *
 *  We shift addr by 32 to knock out the first 32 bits then AND it by 0xFFFFFFFF to get the 
 *  high 32 bits.
 *
 */


void idt_set_vec(uint8_t vec, void* isr, int flags) {
    uint64_t addr = (uint64_t)isr;
    idt[vec].isr_low16 = addr & 0xFFFF;                 // Check description 1.
    idt[vec].isr_mid16 = (addr >> 16) & 0xFFFF;         // Check description 2.
    idt[vec].isr_high32 = (addr >> 32) & 0xFFFFFFFF;    // Check description 3.
    idt[vec].zero = 0;                                  // Must stay zero.
    idt[vec].zero1 = 0;                                 // Must stay zero.
    idt[vec].zero2 = 0;                                 // Must stay zero.
    idt[vec].attr = flags;                              // Flags that specifiy type of interrupt.
    idt[vec].dpl = 0;                                   // Max privilege level that can access this resource.
    idt[vec].p = 1;                                     // Yes, it is present in memory.
    idt[vec].reserved = 0;                              // Reserved, keep zero.
    idt[vec].cs = 0x08;                                 // Code selector.
}


void idt_install() {
    idt_ptr.base = (uint64_t)&idt;
    idt_ptr.limit = sizeof(struct IDTVector) * 256 - 1;
    __asm__ __volatile__("lidt %0" :: "m" (idt_ptr));
}
