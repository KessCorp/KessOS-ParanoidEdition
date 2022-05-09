#include <arch/cpu/lapic/lapic.h>
#include <arch/cpu/cpuid.h>
#include <arch/cpu/msr/msr.h>
#include <arch/io/io.h>
#include <arch/io/legacy-pic.h>
#include <debug/log.h>
#include <util/kernflags.h>
#include <stdint.h>


static uint64_t apic_base = 0x0;


static inline uint32_t lapic_supported() {
    uint32_t eax, unused, edx;
    __get_cpuid(1, &eax, &unused, &unused, &edx);
    return edx & CPUID_FEAT_EDX_APIC;
}


void lapic_init() {
    log("Checking if your processor has a Local APIC chip built in..\n", S_INFO);
    
    // Perform a check to see if LAPIC is supported.
    if (!(lapic_supported())) {
        log("Processor does not have LAPIC. Unsetting USING_APIC bit in kern_flags..\n", S_WARNING);
        kern_flags &= ~(USING_APIC);
        return;
    }

    uint32_t lo;
    uint32_t hi;

    log("Processor has an LAPIC.\n", S_INFO); 
    log("Disabling legacy PIC..\n", S_INFO);
    outportb(PIC1_DATA, 0xFF);
    outportb(PIC2_DATA, 0xFF);
    log("Fetching APIC base from MSR 0x1B (bits 12-35)..\n", S_INFO); 
    rdmsr(0x1B, &lo, &hi);

    // Extract bits 12-35.
    lo >>= 12;
    apic_base = lo;
    apic_base = (apic_base << 4) | (hi & 0xF);
    log("APIC BASE: %x\n", S_INFO, apic_base);
}
