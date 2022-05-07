#ifndef IDT_H
#define IDT_H

#include <stdint.h>


// 2022 Ian Moffett <ian@kesscoin.com>


#define TRAP_GATE_FLAGS 0x8F
#define INT_GATE_FLAGS 0x8E
#define IDT_INT_GATE_USER 0xEE


// Entry inside interrupt discriptor table
// that describes an interrupt.
struct IDTVector {    
    uint16_t isr_low16;         // Low 16 bits of ISR.
    uint16_t cs;                // Code segment selector.
    uint8_t ist : 3;            // Interrupt stack table.
    uint8_t zero : 1;           // Leave zero.
    uint8_t zero1 : 3;          // Leave zero.
    uint8_t attr : 4;           // Flags.
    uint8_t zero2 : 1;          // Leave 0.
    uint8_t dpl : 2;            // Max privilege that can access this.
    uint8_t p : 1;              // Present?
    uint16_t isr_mid16;         // Middle 16 bits of ISR. 
    uint32_t isr_high32;        // High 32 bits of ISR.
    uint32_t reserved;          // Reserved by CPU manufacturer.
};


// Stack frame that will be pushed on interrupt.
struct InterruptFrame {
    uint64_t rip;
    uint64_t cs;
    uint64_t flags;
    uint64_t rsp;
    uint64_t ss;
};


struct __attribute__((packed)) IDTPtr {
    uint16_t limit;
    uint64_t base;
};


void idt_set_vec(uint8_t vec, void* isr, int flags);
void idt_install();


#endif
