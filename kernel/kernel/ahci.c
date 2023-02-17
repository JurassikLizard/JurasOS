#include <kernel/ahci.h>

#include <kernel/pci.h>
#include <kernel/kheap.h>
#include <kernel/paging/pfa.h>
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

            if(port_type == SATA || port_type == SATAPI) {
                driver->ports[driver->port_count] = (ahci_port_t *)kmalloc(sizeof(ahci_port_t));

                ahci_port_t *ahci_port = driver->ports[driver->port_count];
                ahci_port->port_type = port_type;
                ahci_port->hba_port = &driver->ABAR->ports[i];
                ahci_port->port_num = port_count;
                port_count++;
            }
        }
    }
}

void ahci_port_start(ahci_port_t *port) {
    while(port->hba_port->cmd & HBA_PxCMD_CR); // Doing something

    port->hba_portort->cmd |= HBA_PxCMD_FRE;
    port->hba_port->cmd |= HBA_PxCMD_ST;
}

void ahci_port_stop(ahci_port_t *port) {
    port->hba_port->cmd &= ~HBA_PxCMD_ST;
    port->hba_port->cmd &= ~HBA_PxCMD_FRE;

    while(1) {
        if(port->hba_port->cmd & HBA_PxCMD_FR) continue;
        if(port->hba_port->cmd & HBA_PxCMD_CR) continue;

        break;
    }
}

void ahci_configure_port(ahci_port_t *port) {
    ahci_port_stop(port);

    void *new_base = pfa_request_page();
    port->hba_port->clb = (uint32_t)(uint64_t)new_base;
    port->hba_port->clbu = (uint32_t)((uint64_t)new_base >> 32);
    memset((uint8_t *)(port->hba_port->clb), 0, 1024);

    void *fis_base = pfa_request_page();
    port->hba_port->fb = (uint32_t)(uint64_t)fis_base;
    port->hba_port->fbu = (uint32_t)((uint64_t)fis_base >> 32);
    memset((uint8_t *)port->hba_port->fb, 0, 256);

    HBA_CMD_HEADER *cmd_header = (HBA_CMD_HEADER *)((uint64_t)port->hba_port->clb + (uint64_t)port->hba_port->clbu << 32);

    for (int i = 0; i < 32; i++) {
        cmd_header[i].prdtl = 8;

        void *cmd_table_addr = pfa_request_page();
        uint64_t addr = (uint64_t)cmd_table_addr + (i << 8);
        cmd_header[i].ctba = (uint32_t)addr;
        cmd_header[i].ctbau = (uint32_t)((uint64_t)addr >> 32);
        memset(cmd_table_addr, 0, 256);
    }

    ahci_port_start(port);
}

void ahci_driver_init(pci_device_header_t *pci_base_addr) {
    ahci_driver.pci_base_addr = pci_base_addr;

    ahci_driver.ABAR = (HBA_MEM *)((pci_header0_t *)pci_base_addr)->BAR5;
    ptm_map_addr((uint32_t)ahci_driver.ABAR, (uint32_t)ahci_driver.ABAR, 1, 1);

    ahci_driver_probe_ports(&ahci_driver);

    for(int i = 0; i < ahci_driver.port_count; i++) {
        ahci_port_t *port = ahci_driver.ports[i];

        printf("%d", port->port_type);

        ahci_configure_port(port);
    }
}

void ahci_driver_deinit(pci_device_header_t *pci_base_addr) {

}