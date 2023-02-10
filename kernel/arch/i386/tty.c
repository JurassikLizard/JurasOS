#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
	return fg | bg << 4;
}

uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

void terminal_initialize(uint8_t term_col) {
	if (term_col == NULL || !term_col) term_col=vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = term_col;
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll() {
	memmove(terminal_buffer, terminal_buffer + VGA_WIDTH, VGA_WIDTH * (VGA_HEIGHT - 1) * sizeof(uint16_t));
	size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH;
	for(size_t x = 0; x < VGA_WIDTH; ++x)
	{
		terminal_buffer[index + x] = vga_entry(' ', terminal_color);
	}
}

void terminal_delete_line(int line) {
	memset((uint8_t *)(VGA_MEMORY + VGA_WIDTH * line), 0, VGA_WIDTH * 2 - 1);
}

void terminal_putchar(char c) {
	unsigned char uc = c;

	if (uc == '\n'){
		terminal_row += 1;
		terminal_column = 0;
	}
	else if (uc == '\t') {
		terminal_column += 4 - (terminal_row % 4);
	}
	else if (uc == '\b') {
		if(terminal_column == 0) {
			terminal_column = VGA_WIDTH - 1;
			terminal_row--;
		}
		else {
			terminal_column--;
		}
		terminal_putentryat('\0', terminal_color, terminal_column, terminal_row);
	}
	else{
		terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
		terminal_column += 1;
	}

	if (terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		terminal_row += 1;
	}
	if (terminal_row == VGA_HEIGHT)
	{
		// TODO: Fix later
		terminal_scroll();
		terminal_row = VGA_HEIGHT - 1;
	}
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}