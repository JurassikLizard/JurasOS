// Read and write from http://www.osdever.net/tutorials/view/lba-hdd-access-via-pio

#include <kernel/disk.h>

#include <stdint.h>
#include <io.h>


void *ata_write_sector(uint8_t *buffer, uint32_t lba) {
    io_outb(0x1F1, 0x00);

    // Send sector read size
    io_outb(0x1F2, 0x01);

    // Send LBA address
    io_outb(0x1F3, (uint8_t)lba);
    io_outb(0x1F4, (uint8_t)(lba >> 8));
    io_outb(0x1F5, (uint8_t)(lba >> 16));

    // Read sectors command
    io_outb(0x1F7, 0x20);
}