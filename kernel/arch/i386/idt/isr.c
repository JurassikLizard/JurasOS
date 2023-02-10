#include "isr.h"

#include <kernel/idt.h>
#include <kernel/io.h>

#include <stdlib.h>
#include <stdint.h>

/* Can't do this with a loop because we need the address
 * of the function names */
void isr_install() {
    idt_set_gate(0,  (uint32_t)isr0, 0x08, 0x8E);
    idt_set_gate(1,  (uint32_t)isr1, 0x08, 0x8E);
    idt_set_gate(2,  (uint32_t)isr2, 0x08, 0x8E);
    idt_set_gate(3,  (uint32_t)isr3, 0x08, 0x8E);
    idt_set_gate(4,  (uint32_t)isr4, 0x08, 0x8E);
    idt_set_gate(5,  (uint32_t)isr5, 0x08, 0x8E);
    idt_set_gate(6,  (uint32_t)isr6, 0x08, 0x8E);
    idt_set_gate(7,  (uint32_t)isr7, 0x08, 0x8E);
    idt_set_gate(8,  (uint32_t)isr8, 0x08, 0x8E);
    idt_set_gate(9,  (uint32_t)isr9, 0x08, 0x8E);
    idt_set_gate(10,  (uint32_t)isr10, 0x08, 0x8E);
    idt_set_gate(11,  (uint32_t)isr11, 0x08, 0x8E);
    idt_set_gate(12,  (uint32_t)isr12, 0x08, 0x8E);
    idt_set_gate(13,  (uint32_t)isr13, 0x08, 0x8E);
    idt_set_gate(14,  (uint32_t)isr14, 0x08, 0x8E);
    idt_set_gate(15,  (uint32_t)isr15, 0x08, 0x8E);
    idt_set_gate(16,  (uint32_t)isr16, 0x08, 0x8E);
    idt_set_gate(17,  (uint32_t)isr17, 0x08, 0x8E);
    idt_set_gate(18,  (uint32_t)isr18, 0x08, 0x8E);
    idt_set_gate(19,  (uint32_t)isr19, 0x08, 0x8E);
    idt_set_gate(20,  (uint32_t)isr20, 0x08, 0x8E);
    idt_set_gate(21,  (uint32_t)isr21, 0x08, 0x8E);
    idt_set_gate(22,  (uint32_t)isr22, 0x08, 0x8E);
    idt_set_gate(23,  (uint32_t)isr23, 0x08, 0x8E);
    idt_set_gate(24,  (uint32_t)isr24, 0x08, 0x8E);
    idt_set_gate(25,  (uint32_t)isr25, 0x08, 0x8E);
    idt_set_gate(26,  (uint32_t)isr26, 0x08, 0x8E);
    idt_set_gate(27,  (uint32_t)isr27, 0x08, 0x8E);
    idt_set_gate(28,  (uint32_t)isr28, 0x08, 0x8E);
    idt_set_gate(29,  (uint32_t)isr29, 0x08, 0x8E);
    idt_set_gate(30,  (uint32_t)isr30, 0x08, 0x8E);
    idt_set_gate(31,  (uint32_t)isr31, 0x08, 0x8E);
}

/* To print the message which defines every exception */
char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

char *PANIC_MESSAGE = 
"Interrupt %d: %s\n\n"
"Registers:\n"
"edi: %X, esi: %X, ebp: %X\nesp: %X, ebx: %X, edx: %X\necx: %X, eax: %X";

void fault_handler(registers_t regs) {
    PANIC(PANIC_MESSAGE, regs.int_no, exception_messages[regs.int_no],
    regs.edi, regs.esi, regs.ebp, regs.esp, regs.ebx, regs.edx, regs.ecx, regs.eax);
}