#include <arch/cpu/lapic/lapic.h>
#include <arch/cpu/cpuid.h>
#include <arch/io/io.h>
#include <arch/io/legacy-pic.h>
#include <firmware/acpi/tables.h>
#include <debug/log.h>
#include <util/kernflags.h>
#include <stdint.h>

static volatile struct __attribute__((packed)) LAPICRegs {
    uint32_t reserved;              // RESERVED.
    uint32_t id;                    // LAPIC ID.
    uint32_t version;               // LAPIC VERSION.
    uint32_t reserved1;             // RESERVED.
    uint32_t tpr;                   // TASK PRIORITY REGISTER.
    uint32_t apr;                   // ARBIRATION PRIORITY REGISTER.
    uint32_t ppr;                   // PROCESSOR PRIORITY REGISTER.
    uint32_t eoi;                   // END OF INTERRUPT.
    uint32_t rrd;                   // REMOTE READ REGISTER.
    uint32_t logical_dest;          // LOGICAL DEST.
    uint32_t dest_fmt;              // DEST FORMAT REGISTER.
    uint32_t siv;                   // SPURIOUS INTERRUPT VECTOR.
    uint32_t isr;                   // IN-SERVICE REGISTER.
    uint32_t tmr;                   // TRIGGER MODE REGISTER.
    uint32_t irr;                   // INTERRUPT REQUEST REGISTER.
    uint32_t error;                 // ERROR STATUS REGISTER.
    uint32_t reserved2;             // RESERVED.
    uint32_t lvt_cmci;              // LVT CORRECTED MACHINE CHECK INTERRUPT REGISTER.
    uint32_t icr;                   // INTERRUPT COMMAND REGISTER.
    uint32_t lvt_timer;             // LVT TIMER REGISTER. 
    uint32_t lvt_thermal_sensor;    // LVT THERMAL SENSOR REGISTER.
    uint32_t lvt_pmcr;              // LVT PERFORMACE MONITORING COUNTERS REGISTER.
    uint32_t lvt_lint0;             // LVT LINT0 REGISTER.
    uint32_t lvt_lint1;             // LVT LINT1 REGISTER.
    uint32_t lvt_error;             // LVT ERROR REGISTER.
    uint32_t timer_icr;             // TIMER INITIAL COUNT REGISTER.
    uint32_t timer_ccr;             // TIMER CURRENT COUNT REGISTER.
    uint32_t reserved3;             // RESERVED.
    uint32_t timer_divide_config;   // TIMER DIVIDE CONFIG REGISTER.
    uint32_t reserved4;             // RESERVED.
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
