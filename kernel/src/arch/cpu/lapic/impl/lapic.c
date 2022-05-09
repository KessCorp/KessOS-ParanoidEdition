#include <arch/cpu/lapic/lapic.h>
#include <arch/cpu/cpuid.h>
#include <arch/io/io.h>
#include <arch/io/legacy-pic.h>
#include <firmware/acpi/tables.h>
#include <debug/log.h>
#include <util/kernflags.h>
#include <stdint.h>

static volatile struct __attribute__((packed)) LAPICRegs {
    uint32_t reserved;
    uint32_t id;
    uint32_t version;
    uint32_t reserved1;
    uint32_t tpr;
    uint32_t apr;
    uint32_t ppr;
    uint32_t eoi;
    uint32_t rrd;
    uint32_t logical_dest;
    uint32_t siv;
    uint32_t isr;
    uint32_t tmr;
    uint32_t irr;
    uint32_t error;
    uint32_t reserved2;
    uint32_t lvt_cmci;
    uint32_t icr;
    uint32_t lvt_timer;
    uint32_t lvt_thermal_sensor;
    uint32_t lvt_pmcr;
    uint32_t lvt_lint0;
    uint32_t lvt_lint1;
    uint32_t lvt_error;
    uint32_t timer_icr;
    uint32_t timer_ccr;
    uint32_t reserved3;
    uint32_t timer_divide_config;
    uint32_t reserved4;
} *lapic_regs;


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
