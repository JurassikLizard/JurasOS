#include <kernel/gdt.h>

#include <stdio.h>
#include <stdint.h>

__attribute__((aligned(0x1000)))
struct gdt default_gdt = {
    {0, 0, 0, 0x00, 0x00, 0}, // Null
    {0xFFFF, 0, 0, 0x9A, 0xFC, 0}, // Kernel Code 0x08
    {0xFFFF, 0, 0, 0x92, 0xFC, 0}, // Kernel Data 0x10
    {0xFFFF, 0, 0, 0xFA, 0xFC, 0}, // User Code
    {0xFFFF, 0, 0, 0xF2, 0xFC, 0},  // User Data
};