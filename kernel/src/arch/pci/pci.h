#ifndef PCI_H
#define PCI_H

// 2022 Ian Moffett <ian@kesscoin.com>

#include <stdint.h>

#define PCI_HIGHEST_BUS 255
#define PCI_HIGHEST_SLOT 32
#define PCI_HIGHEST_FUNC 8

uint8_t pci_device_exists(uint16_t vendor_id);
uint16_t pci_get_vendor_id(uint8_t bus, uint8_t slot, uint8_t func);
uint16_t pci_get_device_id(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_revision_id(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_prog_if(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_subclass_id(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_classid(uint8_t bus, uint8_t slot, uint8_t func);
uint32_t pci_get_bar0(uint8_t bus, uint8_t slot, uint8_t func);
uint32_t pci_get_bar1(uint8_t bus, uint8_t slot, uint8_t func);
uint32_t pci_get_bar2(uint8_t bus, uint8_t slot, uint8_t func);
uint32_t pci_get_bar3(uint8_t bus, uint8_t slot, uint8_t func);
uint32_t pci_get_bar4(uint8_t bus, uint8_t slot, uint8_t func);

#endif
