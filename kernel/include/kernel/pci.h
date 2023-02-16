#ifndef _PCI_H
#define _PCI_H 1

#include <stdint.h>

#include <kernel/acpi.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __attribute__((packed)) {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_interface;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t BIST;
} pci_device_header_t;

typedef struct __attribute__((packed)) {
    pci_device_header_t device_header;
    uint32_t BAR0;
    uint32_t BAR1;
    uint32_t BAR2;
    uint32_t BAR3;
    uint32_t BAR4;
    uint32_t BAR5;
    uint32_t cardbus_CIS_ptr;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t expansion_rom_base_addr;
    uint8_t cap_ptr; // Capabilities ptr
    uint8_t rsv0;
    uint16_t rsv1;
    uint32_t rsv2;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t mind_grant;
    uint8_t max_latency;
} pci_header0_t;

void pci_enumerate(acpi_mcfg_header_t *mcfg);

const char *pci_get_class_name(uint8_t class_code);
const char *pci_get_vendor_name(uint16_t vendor_id);
const char *pci_get_device_name(uint16_t vendor_id, uint16_t device_id);
const char *pci_get_subclass_name(uint8_t class_code, uint8_t sub_class);
const char *pci_get_prog_interface_name(uint8_t class_code, uint8_t subclass, uint8_t prog_interface);

#ifdef __cplusplus
}
#endif

#endif