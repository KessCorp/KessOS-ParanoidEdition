#include <arch/pci/pci.h>
#include <arch/io/io.h>

// 2022 Ian Moffett <ian@kesscoin.com>

#define CONFIG_ADDR 0xCF8
#define CONFIG_DATA 0xCFC


static uint16_t pci_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;

    // Create config address.
    uint32_t addr = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write address to CONFIG_ADDR
    outportl(CONFIG_ADDR, addr);
    uint16_t ret = (uint16_t)((inportl(CONFIG_DATA) >> ((offset & 2) * 2)) & 0xFFFF);
    return ret;
}


uint8_t pci_valid_vendor(uint16_t vendor_id) {
    return vendor_id != 0xFFFF;
}



uint16_t pci_get_vendor_id(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x0);
}


uint16_t pci_get_device_id(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x2);
}


uint8_t pci_get_revision_id(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x8) & 0xFF;
}


uint8_t pci_get_prog_if(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x8) >> 8; 
}


uint8_t pci_get_subclass_id(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0xA) & 0xFF;
}


uint8_t pci_get_classid(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0xA) >> 8;
}


uint32_t pci_get_bar0(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x13) << 16 | pci_read_word(bus, slot, func, 0x10);
}


uint32_t pci_get_bar1(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x16) << 16 | pci_read_word(bus, slot, func, 0x14);
}


uint32_t pci_get_bar2(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x1A) << 16 | pci_read_word(bus, slot, func, 0x18);
}


uint32_t pci_get_bar3(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x1E) << 16 | pci_read_word(bus, slot, func, 0x1C);
}

uint32_t pci_get_bar4(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x22) << 16 | pci_read_word(bus, slot, func, 0x20);
}


uint32_t pci_get_bar5(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x26) << 16 | pci_read_word(bus, slot, func, 0x24);
}
