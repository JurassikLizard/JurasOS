// Borrows code from Poncho's S2 64-Bit OS Tutorials
#include <kernel/paging/pfa.h>
#include <kernel/paging/paging.h>
#include <kernel/multiboot2.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

uint32_t free_memory; // Entire system free
uint32_t reserved_memory; // Reserved by anything
uint32_t used_memory; // Memory that we have allocated
bool initialized = false;

multiboot_memory_map_t *mmap;

page_frame_allocator_t pfa;
extern uint32_t KERNEL_END;

// Private functions
void init_bitmap(size_t bitmap_size, void *buffer_address);
void pfa_reserve_page(void *address);
void pfa_unreserve_page(void *address);
void pfa_reserve_pages(void *address, uint32_t page_count);
void pfa_unreserve_pages(void *address, uint32_t page_count);

void pfa_read_multiboot_memory_map(unsigned long addr) {
    if (initialized) return;

    // Setup basic values
    // void *largest_free_entry = NULL;
    // uint32_t largest_free_entry_len = 0;

    uint32_t memory_size = 0;
    
    struct multiboot_tag *tag;
    struct multiboot_tag *mmap_tag = 0;

    // https://www.gnu.org/software/grub/manual/multiboot2/html_node/kernel_002ec.html#kernel_002ec
    for (tag = (struct multiboot_tag *) (addr + 8);
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
                                       + ((tag->size + 7) & ~7)))
    {
        if (tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
            mmap_tag = tag;
            for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
                 (multiboot_uint8_t *) mmap 
                   < (multiboot_uint8_t *) tag + tag->size;
                 mmap = (multiboot_memory_map_t *) 
                   ((unsigned long) mmap
                    + ((struct multiboot_tag_mmap *) tag)->entry_size))
            {
                memory_size += (mmap->len & 0xffffffff);
                if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
                    free_memory += (mmap->len & 0xffffffff);
                }
            }
            // Might be source of bugs because I didn't understand this right.
        }
    }

    if (mmap_tag == 0) {
        PANIC("MMAP NOT FOUND!");
    }

    // Loop through all entries to find biggest one
    // multiboot_mmap_entry_t *entry = (multiboot_mmap_entry_t*) mbt->mmap_addr;
    // while((uint32_t)entry < mbt->mmap_addr + mbt->mmap_length) {
	// 	if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
    //         memory_size += entry->len_low;
            
    //         if (largest_free_entry_len < entry->len_low) {
    //             largest_free_entry = (void *) entry->base_addr_low;
    //             largest_free_entry_len = entry->len_low;
    //         }
    //     }

    //     entry = (multiboot_mmap_entry_t *) ((uint32_t) entry + entry->size + sizeof(entry->size));
	// }

    // Initialize bitmap and make sure to not overwrite kernel
    uint32_t bitmap_size = memory_size / 4096 / 8 + 1; // Each page (/ 4096) can be stored as a single bit (/ 8)
    init_bitmap(bitmap_size, (void *)KERNEL_END);
    
    // Lock pages of bitmap buffer
    pfa_lock_pages(pfa.page_bitmap.buffer, NUM_PAGES(pfa.page_bitmap.size));

    // Increment kernel end for the ptm_init function to map the bitmap
    KERNEL_END += bitmap_size;

    for (mmap = ((struct multiboot_tag_mmap *) mmap_tag)->entries;
                 (multiboot_uint8_t *) mmap 
                   < (multiboot_uint8_t *) mmap_tag + mmap_tag->size;
                 mmap = (multiboot_memory_map_t *) 
                   ((unsigned long) mmap
                    + ((struct multiboot_tag_mmap *) mmap_tag)->entry_size))
    {
        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE) {
            pfa_reserve_pages((void *)mmap, NUM_PAGES((mmap->len & 0xffffffff)));
        }
    }

    // Reserve pages of unusable/reserved memory;
    // entry = (multiboot_mmap_entry_t*) mbt->mmap_addr;
    // while((uint32_t)entry < mbt->mmap_addr + mbt->mmap_length) {
    //     if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
    //         pfa_reserve_pages(entry, NUM_PAGES(entry->len_low));
    //     }
        
    //     entry = (multiboot_mmap_entry_t *) ((uint32_t) entry + entry->size + sizeof(entry->size));
	// }

    // MAKE SURE this is at the bottom of initialization so there aren't any race conditions
    // Just in case ^^ not entirely sure
    initialized = true;
}

void init_bitmap(size_t bitmap_size, void *buffer_address) {
    pfa.page_bitmap.size = bitmap_size;
    pfa.page_bitmap.buffer = (uint8_t *)buffer_address;

    for (uint32_t i = 0; i < bitmap_size; i++) {
        pfa.page_bitmap.buffer[i] = 0;
    }
}

__attribute__((assume_aligned(4096)))
void *pfa_request_page() {
    for (uint32_t index = 0; index < pfa.page_bitmap.size * 8; index++) {
        if (bitmap_get(&pfa.page_bitmap, index) == true) continue;

        pfa_lock_page((void *) (index * 4096));
        return (void *) (index * 4096);
    }

    // TODO: Add page swap to file system
    return NULL;
}

// Very bad implementation and should be redone allow for non-sequential blocks of memory to be allocated
__attribute__((assume_aligned(4096)))
void *pfa_request_pages(uint32_t page_count) {
    for (uint32_t index = 0; index < pfa.page_bitmap.size * 8; index++) {
        uint8_t broken_memory_flag = 0;
        for (uint32_t offset = 0; offset < page_count && (offset + index) < pfa.page_bitmap.size * 8; offset++){
            if (bitmap_get(&pfa.page_bitmap, index + offset) == true) { // Not contiguous freed memory
                broken_memory_flag = 1;
                index = index + offset; // To prevent looping through the same chunks that aren't available
                break; // Breaks offset loop
            }
        }

        if(broken_memory_flag) continue; // continue on to next memory chunk (in this case it is the index + offset one)

        pfa_lock_pages((void *) (index * 4096), page_count);
        return (void *) (index * 4096);
    }

    // TODO: Add page swap to file system
    return NULL;
}

__attribute__((assume_aligned(4096)))
void *pfa_request_mpage() {
    for (uint32_t index = 0; index < pfa.page_bitmap.size * 8; index++) {
        if (bitmap_get(&pfa.page_bitmap, index) == true) continue;
        void *page = (void *)(index * 4096);

        ptm_map_addr((uint32_t)page, (uint32_t)page, true, true);
        pfa_lock_page(page);
        return page;
    }

    // TODO: Add page swap to file system
    return NULL;
}

__attribute__((assume_aligned(4096)))
void *pfa_request_mpages(uint32_t page_count) {
    for (uint32_t index = 0; index < pfa.page_bitmap.size * 8; index++) {
        uint8_t broken_memory_flag = 0;
        for (uint32_t offset = 0; offset < page_count && (offset + index) < pfa.page_bitmap.size * 8; offset++){
            if (bitmap_get(&pfa.page_bitmap, index + offset) == true) { // Not contiguous freed memory
                broken_memory_flag = 1;
                index = index + offset; // To prevent looping through the same chunks that aren't available
                break; // Breaks offset loop
            }
        }

        if(broken_memory_flag) continue; // continue on to next memory chunk (in this case it is the index + offset one)

        void *page = (void *)(index * 4096);

        for (uint32_t offset = 0; offset < page_count; offset++) {
            ptm_map_addr((uint32_t)page + (offset * 4096), (uint32_t)page + (offset * 4096), true, true);
        }

        pfa_lock_pages(page, page_count);
        return page;
    }

    // TODO: Add page swap to file system
    return NULL;
}

void pfa_free_page(void *address) {
    uint32_t index = (uint32_t) address / 4096;
    
    if (bitmap_get(&pfa.page_bitmap, index) == false) return; // Already free
    
    // Set and update memory counts
    bitmap_set(&pfa.page_bitmap, index, false);
    free_memory += 4096;
    used_memory -= 4096;
}

void pfa_lock_page(void *address) {
    uint32_t index = (uint32_t) address / 4096;
    
    if (bitmap_get(&pfa.page_bitmap, index) == true) return; // Already free
    
    // Set and update memory counts
    bitmap_set(&pfa.page_bitmap, index, true);
    free_memory -= 4096;
    used_memory += 4096;
}

void pfa_free_pages(void *address, uint32_t page_count) {
    for (uint32_t i = 0; i < page_count; i++) {
        pfa_free_page((void *)( (uint32_t)address + (i * 4096) ));
    }
}

void pfa_lock_pages(void *address, uint32_t page_count) {
    for (uint32_t i = 0; i < page_count; i++) {
        pfa_lock_page((void *)( (uint32_t)address + (i * 4096) ));
    }
}

void pfa_unreserve_page(void *address) {
    uint32_t index = (uint32_t) address / 4096;
    
    if (bitmap_get(&pfa.page_bitmap, index) == false) return; // Already free
    
    // Set and update memory counts
    bitmap_set(&pfa.page_bitmap, index, false);
    reserved_memory -= 4096;
}

void pfa_reserve_page(void *address) {
    uint32_t index = (uint32_t) address / 4096;
    
    if (bitmap_get(&pfa.page_bitmap, index) == true) return; // Already reserved
    
    // Set and update memory counts
    bitmap_set(&pfa.page_bitmap, index, true);
    reserved_memory += 4096;
}

void pfa_unreserve_pages(void *address, uint32_t page_count) {
    for (uint32_t i = 0; i < page_count; i++) {
        pfa_unreserve_page((void *)( (uint32_t)address + (i * 4096) ));
    }
}

void pfa_reserve_pages(void *address, uint32_t page_count) {
    for (uint32_t i = 0; i < page_count; i++) {
        pfa_reserve_page((void *)( (uint32_t)address + (i * 4096) ));
    }
}

uint32_t pfa_free_memory() {
    return free_memory;
}

uint32_t pfa_reserved_memory() {
    return reserved_memory;
}

uint32_t pfa_used_memory() {
    return used_memory;
}