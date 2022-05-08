#include <firmware/acpi/acpi.h>
#include <firmware/acpi/tables.h>
#include <util/string.h>
#include <util/asm.h>
#include <util/kernflags.h>
#include <debug/log.h>
#include <stddef.h>

#define RSDP_SIGNATURE "RSD PTR"
#define APIC_SIGNATURE "APIC"


// 2022 Ian Moffett <ian@kesscoin.com>

static acpi_rsdt_t* rsdt;
acpi_madt_t* apic_madt = NULL;


static void verify_rsdt() {
    unsigned char sum = 0;

    for (uint32_t i = 0; i < rsdt->header.length; ++i) {
        sum += ((char*)&rsdt->header)[i];
    }

    // If the sum mod 0x100 is not zero, then we have an invalid checksum.
    if (sum % 0x100 != 0) {
        log("RSDT checksum invalid, system halted. (Continuing without ACPI).", S_CRITICAL);
        kern_flags &= ~(USING_ACPI);                // We can't use ACPI so unset the bit.
    }
}


static inline uint32_t rsdt_entry_count() {
    return (rsdt->header.length - sizeof(rsdt->header)) / 4;
}


// This will locate the MADT structure.
static void find_madt() {
    for (uint32_t i = 0; i < rsdt_entry_count(); ++i) {
        acpi_madt_t* current = (acpi_madt_t*)(uint64_t)rsdt->tables[i];

        if (strncmp(current->header.signature, APIC_SIGNATURE, strlen(APIC_SIGNATURE))) {
            apic_madt = current;
            kern_flags |= USING_APIC;       // We are using APIC if we found MADT.
            log("MADT found!\n", S_INFO);
            break;
        }
    }

    // Just display a notice if it is not found, and we can just use legacy PIC.
    if (!(kern_flags & USING_APIC))
        log("MADT not found, continuing without APIC.\n", S_WARNING);
}


void acpi_init(void* rsdp) {
    acpi_rsdp_t* const RSDP = (acpi_rsdp_t*)rsdp;

    log("Verifying RSDP signature..\n", S_INFO);
    if (!(strncmp(RSDP->signature, RSDP_SIGNATURE, strlen(RSDP_SIGNATURE)))) {
        log("RSDP is invalid, signature does not match! (Continuing without ACPI).\n", S_CRITICAL);
        kern_flags &= ~(USING_ACPI);            // We are no longer using ACPI, unset the bit.
        return;
    }

    log("RSDP signature is valid.\n", S_INFO);
    rsdt = (acpi_rsdt_t*)(uint64_t)RSDP->rsdtaddr;

    log("Veriying RSDT checksum..\n", S_INFO);
    verify_rsdt();

    // Check if the USING_ACPI bit is still set.
    if (!(kern_flags & USING_ACPI))
        // No, so do not continue.
        return;

    log("RSDT checksum valid!\n", S_INFO);      // If it reached here without halting, it is valid.
    log("Locating MADT..\n", S_INFO);
    find_madt();
    log("ACPI setup finished!\n", S_INFO);
}
