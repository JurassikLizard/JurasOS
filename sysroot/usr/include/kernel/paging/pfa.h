// Heavily borrowed from Poncho's S2 64-Bit OS Tutorials

#ifndef _PAGE_FRAME_ALLOCATOR_H
#define _PAGE_FRAME_ALLOCATOR_H 1

#include <stdint.h>
#include <kernel/bitmap.h>
#include <kernel/multiboot.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_PAGES(x) ((uint32_t)x / 4097 + 1)

typedef struct page_frame_allocator_struct {
    bitmap_t page_bitmap;
} page_frame_allocator_t;

void pfa_read_multiboot_memory_map(multiboot_info_t* mbt);

void pfa_free_page(void *address);
void pfa_lock_pages(void *address, uint32_t page_count);
void pfa_free_page(void *address);
void pfa_lock_pages(void *address, uint32_t page_count);

void *pfa_request_page();
void *pfa_request_pages(uint32_t page_count);
// Create page and identity map it
void *pfa_request_mpage();
void *pfa_request_mpages(uint32_t page_count);

uint32_t pfa_free_memory();
uint32_t pfa_reserved_memory();
uint32_t pfa_used_memory();

#ifdef __cplusplus
}
#endif

#endif