#ifndef _IDT_H
#define _IDT_H 1

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct idt_entry
{
  uint16_t offset_low;
  uint16_t selector;
  uint8_t zero;
  uint8_t type_attr;
  uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr
{
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

typedef struct {
  uint32_t ds;                  // Data segment selector
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
  uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
  uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} registers_t;

void idt_install();

void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

#ifdef __cplusplus
}
#endif

#endif