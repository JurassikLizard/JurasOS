// Uses Poncho OS Dev
#include <kernel/pci.h>

#include <kernel/acpi.h>
#include <kernel/ahci.h>
#include <kernel/paging/paging.h>

#include <stdio.h>

// TODO: Determine whether it should be 64 or 32 bit addresses

void pci_enumerate_function(uint64_t device_addr, uint64_t function) {
    uint64_t offset = function << 12;

    uint64_t function_address = device_addr + offset;

    ptm_map_addr((uint32_t)function_address, (uint32_t)function_address, 1, 1);

    pci_device_header_t *pci_device_header = (pci_device_header_t *)function_address;

    if(pci_device_header->device_id == 0) return;
    else if(pci_device_header->device_id == 0xFFFF) return;

    switch (pci_device_header->class_code) {
        case 0x01: // Mass storage controller
            switch (pci_device_header->subclass) {
                case 0x06: // Serial ATA
                    switch (pci_device_header->prog_interface) {
                        case 0x01:
                            // AHCI 1.0 Device
                            extern ahci_driver_t ahci_driver;
                            ahci_driver_init(pci_device_header);
                    }
            }
            break;
    }
}

void pci_enumerate_device(uint64_t bus_addr, uint64_t device) {
    uint64_t offset = device << 15;

    uint64_t device_address = bus_addr + offset;

    ptm_map_addr((uint32_t)device_address, (uint32_t)device_address, 1, 1);

    pci_device_header_t *pci_device_header = (pci_device_header_t *)device_address;

    if(pci_device_header->device_id == 0) return;
    else if(pci_device_header->device_id == 0xFFFF) return;

    for (uint64_t function = 0; function < 8; function++) {
        pci_enumerate_function(device_address, function);
    }
}

void pci_enumerate_bus(uint64_t base_addr, uint64_t bus) {
    uint64_t offset = bus << 20;

    uint64_t bus_address = base_addr + offset;

    ptm_map_addr((uint32_t)bus_address, (uint32_t)bus_address, 1, 1);

    pci_device_header_t *pci_device_header = (pci_device_header_t *)bus_address;

    if(pci_device_header->device_id == 0) return;
    else if(pci_device_header->device_id == 0xFFFF) return;

    for (uint64_t device = 0; device < 32; device++) {
        pci_enumerate_device(bus_address, device);
    }
}

void pci_enumerate(acpi_mcfg_header_t *mcfg) {
    int entries = ((mcfg->header.length) - sizeof(acpi_mcfg_header_t)) / sizeof(device_config_t);
    
    for (int i = 0; i < entries; i++) {
        device_config_t *new_device_config = (device_config_t *)((uint32_t)mcfg + sizeof(acpi_mcfg_header_t) + (sizeof(device_config_t) * i));
        for (uint64_t bus = new_device_config->start_bus; bus < new_device_config->end_bus; bus++) {
            pci_enumerate_bus((uint64_t)new_device_config->base_addr, bus);
        }
    }
}