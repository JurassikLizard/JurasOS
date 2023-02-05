#include <kernel/idt.h>

#include <stdlib.h>

void division_error_handler(void) {
    PANIC("Division Error Encountered!");
}

void double_fault_handler(void) {
    PANIC("Double Fault Encountered!");
}

void page_fault_handler(void) {
    PANIC("Page Fault Encountered!");
}