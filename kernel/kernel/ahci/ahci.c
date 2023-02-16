#include <kernel/ahci.h>

#include <kernel/pci.h>
#include <kernel/paging/paging.h>

#include <stdio.h>

ahci_driver_t ahci_driver;

PORT_TYPE check_port_type(HBA_PORT *port) {
    uint32_t sata_status = port->ssts;

    uint8_t interface_power_mangment = (sata_status >> 8) & 0b111;
    uint8_t device_detection = sata_status & 0b111;

    if(device_detection != HBA_PORT_DEV_PRESET) return None;
    if(interface_power_mangment != HBA_PORT_IPM_ACTIVE) return None;

    switch (port->sig)
    {
        case SATA_SIG_ATAPI:
            return SATAPI;
        case SATA_SIG_ATA:
            return SATA;
        case SATA_SIG_PM:
            return PM;
        case SATA_SIG_SEMB:
            return SEMB;
        default:
            printf("%X", port->sig);
            return None;
    }
}

void ahci_driver_probe_ports(ahci_driver_t *driver) {
    uint32_t ports_impl = driver->ABAR->pi;
    for (int i = 0; i < 32; i++) {
        if (ports_impl & (1 << i)) {
            PORT_TYPE port_type = check_port_type(&driver->ABAR->ports[i]);

            
        }
    }
}

void ahci_driver_init(pci_device_header_t *pci_base_addr) {
    ahci_driver.pci_base_addr = pci_base_addr;

    ahci_driver.ABAR = (HBA_MEM *)((pci_header0_t *)pci_base_addr)->BAR5;
    ptm_map_addr((uint32_t)ahci_driver.ABAR, (uint32_t)ahci_driver.ABAR, 1, 1);

    ahci_driver_probe_ports(&ahci_driver);
}

void ahci_driver_deinit(pci_device_header_t *pci_base_addr) {

}