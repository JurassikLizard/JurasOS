// Heavily borrowed from Poncho's S2 64-Bit OS Tutorials

#ifndef _PAGE_FRAME_ALLOCATOR_H
#define _PAGE_FRAME_ALLOCATOR_H 1

#include <stdint.h>
#include <kernel/bitmap.h>
#include <kernel/multiboot2.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_PAGES(x) ((uint32_t)(x) / 4098 + 1)

typedef struct page_frame_allocator_struct {
    bitmap_t page_bitmap;
} page_frame_allocator_t;

void pfa_read_multiboot_memory_map(unsigned long addr);

// Free 4096 chunk of memory at address
void pfa_free_page(void *address);
// Lock 4096 chunk of memory at address
void pfa_lock_page(void *address);

// Free 4096 * page_count chunk of memory at address
void pfa_free_pages(void *address, uint32_t page_count);
// Lock 4096 * page_count chunk of memory at address
void pfa_lock_pages(void *address, uint32_t page_count);

// Find page
__attribute__((assume_aligned(4096)))
void *pfa_request_page();
// Find contiguous page_count pages
__attribute__((assume_aligned(4096)))
void *pfa_request_pages(uint32_t page_count);
// Find page and identity map it
__attribute__((assume_aligned(4096)))
void *pfa_request_mpage();
// Find contiguous page_count pages and identity map them
__attribute__((assume_aligned(4096)))
void *pfa_request_mpages(uint32_t page_count);

// Get free memory available
uint32_t pfa_free_memory();
// Get reserved memory (reserved by system)
uint32_t pfa_reserved_memory();
// Get locked memory used by kernel
uint32_t pfa_used_memory();

#ifdef __cplusplus
}
#endif

#endif