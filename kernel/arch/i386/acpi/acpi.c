// From Poncho's tutorial
#include <kernel/acpi/acpi.h>

#include <kernel/multiboot2.h>
#include <kernel/tags.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// TODO: Validate checksum of RSDP, RSDT, and ACPI Tables
// Only uses RSDP <2.0 for now
void acpi_init(multiboot_tags_info_t *tags) {
    if(!tags->acpi_old) PANIC("NO OLD ACPI");

    acpi_sdt_header_t *rsdt = (acpi_sdt_header_t *)(((acpi_rsdp_t *)(tags->acpi_old->rsdp))->rsdt_address);
    
    acpi_mcfg_header_t *mcfg = (acpi_mcfg_header_t *)acpi_find_table(rsdt, "MCFG");
}

void *acpi_find_table(acpi_sdt_header_t *std_header, char *signature) {
    int entries = (std_header->length - sizeof(acpi_sdt_header_t)) / 4;
    for(int i = 0; i < entries; i++) {
        // This is so scuffed lmao
        acpi_sdt_header_t *new_sdt_header = (acpi_sdt_header_t *)*(uint32_t *)((uint32_t)std_header + sizeof(acpi_sdt_header_t) + i * 4);
        if(!strncmp(new_sdt_header->signature, signature, 4))  { // Equal
            return std_header;
        }
    }

    return 0;
}