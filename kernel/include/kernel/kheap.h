// Heavy use of https://wiki.osdev.org/User:Pancakes/BitmapHeapImplementation

#ifndef _KHEAP_H
#define _KHEAP_H 1

#include <stdint.h>
#include <stddef.h>

#include <kernel/bitmap.h>

// x is size of wanted kheap allocation, y is granularity
// Round up
// Src: https://stackoverflow.com/questions/11363304/understanding-ceiling-macro-use-cases
#define KHEAP_BITMAP_SIZE(x, y) (((x) + (y) - 1) / (y))

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kheap_chunk_struct {
    uint32_t size;
	uint32_t used; // amount of memory used (should be aligned to granularity)
	uint16_t granularity;
    uint32_t last_found_block;
    uint8_t *start;
} kheap_chunk_t;

typedef struct kheap_struct {
    bitmap_t bitmap;
    kheap_chunk_t chunk;
} kheap_t;

uint32_t kheap_memory_size_of(size_t memory_size, uint16_t granularity);

// Block_Size needs to include size of kheap_block_t, 
// while bitmap_size needs to reference the portion of memory that doesn't include kheap_block_t
int kheap_add(void *addr, size_t memory_size, uint16_t granularity);

void *kmalloc(size_t size);

void kfree(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif