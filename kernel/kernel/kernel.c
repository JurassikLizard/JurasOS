#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/multiboot.h>
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

void kernel_main(multiboot_info_t* mbt, unsigned int magic) {
	KERNEL_START = (uint32_t)&_kernel_start;
	KERNEL_END = (uint32_t)&_kernel_end;
	terminal_initialize(NULL);


	printf("KERNEL_START: 0x%X, KERNEL_END: 0x%X\n", KERNEL_START, KERNEL_END);

	gdt_install();
	
	/* Make sure the magic number matches and Check bit 6 to see if we have a valid memory map */
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC || !(mbt->flags >> 6 & 0x1)) {
        printf("Multiboot error!"); // TODO: PANIC
    }

	pfa_read_multiboot_memory_map(mbt);

	init_timer(50);
	init_keyboard();

	asm volatile("sti");
  	idt_install();

	uint32_t kheap_addr = KERNEL_END;
	uint32_t kheap_size = 0x100000;
	uint8_t kheap_granularity = 2;
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