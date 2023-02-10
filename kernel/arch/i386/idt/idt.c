#include <kernel/idt.h>

#include "isr.h"
#include "irq.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <kernel/io.h>

struct idt_entry idt[256];
struct idt_ptr idtp;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags)
{
    idt[num].offset_low = base & 0xFFFF;
    idt[num].offset_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].type_attr = flags;
}

void idt_install()
{
    /* Sets the special IDT pointer up, just like in 'gdt.c' */
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t) &idt;

    /* Clear out the entire IDT, initializing it to zeros */
    memset(&idt, 0, sizeof(struct idt_entry) * 256);
    
    isr_install();
    irq_install();

    __asm__ volatile("lidt %0" : : "m"(idtp));
    io_outb(0x21, 0xfd);
    io_outb(0xa1, 0xff);
}