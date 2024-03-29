#ifndef _IRQ_H
#define _IRQ_H 1

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

#include <kernel/idt.h>

#ifdef __cplusplus
extern "C" {
#endif

/* IRQ definitions */
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void irq_install();

void irq_install_handler(int irq, void (*handler)(registers_t regs));
void irq_uninstall_handler(int irq);

#ifdef __cplusplus
}
#endif

#endif