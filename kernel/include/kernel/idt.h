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

void idt_install();

void division_error_handler(void);
void double_fault_handler(void);
void page_fault_handler(void);

#ifdef __cplusplus
}
#endif

#endif