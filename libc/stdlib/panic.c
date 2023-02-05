#include <stdlib.h>

#include <stdio.h>
#include <stdbool.h>

#include <kernel/tty.h>

__attribute__((__noreturn__))
void PANIC(const char *message, ...) {
    bool have_panic = false;
#if defined(__is_libk)
	have_panic = true;
#endif
    if (have_panic) {
        // TODO: Add proper kernel panic.
        va_list args;

        va_start(args, message);

        terminal_initialize(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
        terminal_setcolor(vga_entry_color(VGA_COLOR_RED, VGA_COLOR_WHITE));
	    printf("                                  KERNEL PANIC                                  \n");
        terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
        vprintf(message, args);
        asm volatile("hlt");

        va_end(args);

        while (1) { }
	    __builtin_unreachable();
    }
    
}