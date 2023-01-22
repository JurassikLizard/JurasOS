#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/multiboot.h>
#include <kernel/mmap.h>
#include <kernel/tty.h>
#include <kernel/gdt.h>
#include <kernel/paging/pfa.h>
#include <kernel/paging/paging.h>

extern void *_kernel_start;
extern void *_kernel_end;

uint32_t KERNEL_END;

void kernel_main(multiboot_info_t* mbt, unsigned int magic) {
	KERNEL_END = (uint32_t)&_kernel_end;
	
	terminal_initialize();
	load_gdt(sizeof(struct gdt) - 1, (uint32_t)&default_gdt);
	
	/* Make sure the magic number matches */
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        printf("invalid magic number!"); // TODO: PANIC
    }

	/* Check bit 6 to see if we have a valid memory map */
    if(!(mbt->flags >> 6 & 0x1)) {
        printf("invalid memory map given by GRUB bootloader"); // TODO: PANIC
    }

	printf("KERNEL_END: %p\n", KERNEL_END);

	// mmap_print(mbt);
	
	pfa_read_multiboot_memory_map(mbt);

	//Lock kernel pages
	uint32_t kernel_size = KERNEL_END - (uint32_t)&_kernel_start;
	uint32_t kernel_pages = NUM_PAGES(kernel_size);
	pfa_lock_pages(&_kernel_start, kernel_pages);
	
	printf("Total Memory: %d, Free: %d, Reserved: %d, Used: %d\n", 
		pfa_free_memory() + pfa_reserved_memory() + pfa_used_memory(),
		pfa_free_memory(), pfa_reserved_memory(), pfa_used_memory());
	
	ptm_initialize();
	
	// TODO: Actually implement terminal scrolling
	printf("\nHello, kernel World!\n");
}