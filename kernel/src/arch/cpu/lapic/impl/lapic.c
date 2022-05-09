#include <arch/cpu/lapic/lapic.h>
#include <arch/cpu/cpuid.h>
#include <arch/io/io.h>
#include <arch/io/legacy-pic.h>
#include <firmware/acpi/tables.h>
#include <debug/log.h>
#include <util/kernflags.h>
#include <stdint.h>


enum {
    LAPIC_ID_REG        = 0x20,     // ID REGISTER.
    LAPIC_VERSION_REG   = 0x30,     // VERSION REGISTER.
    LAPIC_TPR           = 0x80,     // TASK PRIORITY REGISTER.
    LAPIC_APR           = 0x90,     // ARBIRATION PRIORITY REGISTER.
    LAPIC_PPR           = 0xA0,     // PROCESSOR PRIORITY REGISTER.
    LAPIC_EOI           = 0xB0,     // END OF INTERRUPT.
    LAPIC_RRD           = 0xC0,     // REMOTE READ REGISTER.
    LAPIC_LDF           = 0xD0,     // LOGICAL DESTINATION FORMAT.
    LAPIC_DEST_FMT      = 0xE0,     // DESTINATION FORMAT.
    LAPIC_SIV           = 0xF0,     // SPURIOUS INTERRUPT VECTOR.
    LAPIC_ISR           = 0x100,    // IN-SERVICE REGISTER.
    LAPIC_TMR           = 0x180,    // TRIGGER MODE REGISTER.
    LAPIC_IRR           = 0x200     // INTERRUPT REQUEST REGISTER.    
};


static uint64_t lapic_base = 0x0;


static inline uint32_t lapic_supported() {
    uint32_t eax, unused, edx;
    __get_cpuid(1, &eax, &unused, &unused, &edx);
    return edx & CPUID_FEAT_EDX_APIC;
}


void lapic_init() {
    extern acpi_madt_t* apic_madt;

    log("Checking if your processor has a Local APIC chip built in..\n", S_INFO);
    
    // Perform a check to see if LAPIC is supported.
    if (!(lapic_supported())) {
        log("Processor does not have LAPIC. Unsetting USING_APIC bit in kern_flags..\n", S_WARNING);
        kern_flags &= ~(USING_APIC);
        return;
    }

    log("Processor has an LAPIC.\n", S_INFO); 
    log("Disabling legacy PIC..\n", S_INFO);
    outportb(PIC1_DATA, 0xFF);
    outportb(PIC2_DATA, 0xFF);
    
    log("Constructing LAPIC register space..\n", S_INFO);
    lapic_base = (uint64_t)apic_madt->lapic_addr;
}
