#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/multiboot2.h>
#include <kernel/tty.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/timer.h>
#include <kernel/kheap.h>
#include <kernel/keyboard.h>
#include <kernel/paging/pfa.h>
#include <kernel/paging/paging.h>

extern void *_kernel_start;
extern void *_kernel_end;

uint32_t KERNEL_START;
uint32_t KERNEL_END;
 
struct multiboot_info {
    uint32_t total_size;
    uint32_t reserved;
    struct multiboot_tag tags[0];
};

void kernel_main(unsigned long magic, unsigned long mbi_addr) {
	KERNEL_START = (uint32_t)&_kernel_start;
	KERNEL_END = (uint32_t)&_kernel_end;
	terminal_initialize(NULL);


	printf("KERNEL_START: 0x%X, KERNEL_END: 0x%X\n", KERNEL_START, KERNEL_END);

	/* Make sure the magic number matches (different than header magic) */
    if(magic != 0x36d76289) {
        PANIC("Multiboot magic not aligned: 0x%X", magic);
    }
	if (mbi_addr & 7)
    {
      PANIC("Unaligned MBI: 0x%X\n", mbi_addr);
    }

	gdt_install();

	pfa_read_multiboot_memory_map(mbi_addr);

	idt_install();

	init_timer(50);
	init_keyboard();

	asm volatile("sti");
	

	uint32_t kheap_addr = KERNEL_END;
	uint32_t kheap_size = 0x80000; // Half a megabyte
	uint8_t kheap_granularity = 4; // uint32_t
	kheap_add((void *)kheap_addr, kheap_size, kheap_granularity);
	printf("Heap location: %p, total size: %d\n", kheap_addr, kheap_memory_size_of(kheap_size, kheap_granularity));

	//KERNEL_END += kheap_memory_size_of(kheap_size, kheap_granularity);
	
	// Make sure to have KERNEL_END be it's final location (everything now should be dynamic memory allocation)
	ptm_initialize();

	printf("After init KERNEL_END: 0x%X\n", KERNEL_END);

	printf("Total Memory: %d, Free: %d, Reserved: %d, Used: %d\n", 
		pfa_free_memory() + pfa_reserved_memory() + pfa_used_memory(),
		pfa_free_memory(), pfa_reserved_memory(), pfa_used_memory());

	printf("\nHello, kernel world!\n");

	for(;;) {
		asm("hlt");
	}
}