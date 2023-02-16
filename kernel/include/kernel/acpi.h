// Heavy use of https://wiki.osdev.org/User:Pancakes/BitmapHeapImplementation

#ifndef _ACPI_H
#define _ACPI_H 1

#include <stdint.h>
#include <stdbool.h>

#include <kernel/tags.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RSDP_10_SIZE (8UL)
// RSDP10 + uint128_t
#define RSDP_20_SIZE (24UL)

// https://wiki.osdev.org/RSDP
typedef struct __attribute__((packed)) {
    char signature[8];
    uint8_t checksum;
    char OEMID[6];
    uint8_t revision;
    uint32_t rsdt_address;
} acpi_rsdp_t;

//https://wiki.osdev.org/RSDT
typedef struct __attribute__((packed)) {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char OEMID[6];
    char OEM_table_id[8];
    uint32_t OEM_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} acpi_sdt_header_t;

//https://wiki.osdev.org/PCI_Express
typedef struct __attribute__((packed)) {
    acpi_sdt_header_t header;
    uint64_t reserved;
} acpi_mcfg_header_t;

//https://wiki.osdev.org/PCI_Express
typedef struct __attribute__((packed)) {
    uint64_t base_addr;
    uint16_t pci_sig_group;
    uint8_t start_bus;
    uint8_t end_bus;
    uint32_t reserved;
} device_config_t;

void acpi_init(multiboot_tags_info_t *tags);

void *acpi_find_table(acpi_sdt_header_t *std_header, char *signature);

#ifdef __cplusplus
}
#endif

#endif