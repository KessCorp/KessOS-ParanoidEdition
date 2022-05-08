#include <arch/cpu/lapic/lapic.h>
#include <arch/cpu/cpuid.h>
#include <debug/log.h>
#include <util/kernflags.h>
#include <stdint.h>


static inline uint8_t lapic_supported() {
    uint32_t unused, edx;
    __get_cpuid(1, &unused, &unused, &unused, &edx);
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
}
