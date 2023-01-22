#ifndef _GDT_H
#define _GDT_H 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct gdt_entry {
    uint16_t limit0;
    uint16_t base0;
    uint8_t base1;
    uint8_t access_byte;
    uint8_t limit1_flags;
    uint8_t base2;
} __attribute__((packed));


struct gdt {
    struct gdt_entry null;
    struct gdt_entry kernel_code_segment;
    struct gdt_entry kernel_data_segment;
    struct gdt_entry user_code_segment;
    struct gdt_entry user_data_segment;
} __attribute__((packed)) __attribute__((aligned(0x1000)));

extern struct gdt default_gdt;

extern void load_gdt(uint32_t limit, uint32_t base);

#ifdef __cplusplus
}
#endif

#endif