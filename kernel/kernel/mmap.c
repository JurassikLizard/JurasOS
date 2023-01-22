#include <kernel/mmap.h>
#include <kernel/multiboot.h>
#include <stdio.h>
#include <stdint.h>

//TODO: Might have to stop relying on just 32-bits of max memory and being able to use only lower hald of 64-bit values

void mmap_print(multiboot_info_t* mbt) {
    uint32_t total_mem = 0;
    multiboot_mmap_entry_t *entry = (multiboot_mmap_entry_t*) mbt->mmap_addr;
    while((uint32_t)entry < mbt->mmap_addr + mbt->mmap_length) {
		printf("--------MEMORY ENTRY--------\n");
        printf("base_addr:0x%x, len:0x%x, type:%d\n",
            entry->base_addr_low, entry->len_low, entry->type);
        total_mem += entry->len_low;
        entry = (multiboot_mmap_entry_t *) ((uint32_t) entry + entry->size + sizeof(entry->size));
	}

    printf("--------------%d--------------\n", total_mem);
}