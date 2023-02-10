#ifndef _IO_H
#define _IO_H 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t io_inb (uint16_t port);
void io_outb (uint16_t port, uint8_t data);
uint16_t io_inw (uint16_t port);
void io_outw (uint16_t port, uint16_t data);

#ifdef __cplusplus
}
#endif

#endif