#include <kernel/kheap.h>

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/bitmap.h>

kheap_t *kheap;

/*
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
*/
// Block_Size needs to include size of kheap_block_t, 
// while bitmap_size needs to reference the portion of memory that doesn't include kheap_block_t and size of bitmap_t
// Addr: address of free memory length kheap_memory_size_of() returns
// Memory_size, size of desired heap chunk
// Granularity size of 1 heap block
// TODO: Use a linked list of some sort so you don't have to have 1 continous memory segment for heap
int kheap_add(void *addr, size_t memory_size, uint16_t granularity) {
    size_t kheap_size = kheap_memory_size_of(memory_size, granularity);
    memset((uint8_t *)addr, 0, kheap_size);

    size_t bitmap_size = KHEAP_BITMAP_SIZE(memory_size, granularity)/8;

    kheap = addr;

    kheap->bitmap.size = bitmap_size;
    kheap->bitmap.buffer = addr + sizeof(kheap_t);

    kheap->chunk.size = memory_size;
    kheap->chunk.granularity = granularity;
    kheap->chunk.start = addr + sizeof(kheap_t) + bitmap_size;

    return 1;
}

size_t kheap_memory_size_of(size_t memory_size, uint16_t granularity) {
    size_t mem = 0;
    mem += sizeof(kheap_t); // Size of bitmap and heap chunk header
    // Size of bitmap
    mem += (KHEAP_BITMAP_SIZE(memory_size, granularity)/8); // 8 bits (entries) can be stored in 1 uint8_t
    mem += memory_size; // Size of heap chunk
    
    return mem;
}

// Everything in heap should already be identity paged
void *kmalloc(size_t size) {
    bitmap_t bitmap = kheap->bitmap;
    size_t block_count = KHEAP_BITMAP_SIZE(size, kheap->chunk.granularity);

    for (uint32_t index = 0; index < bitmap.size * 8; index++) { // * 8 because you want to be able to increment by single digits
        uint8_t broken_memory_flag = 0;
        
        for (uint32_t offset = 0; offset < block_count && (index + offset) < bitmap.size * 8; offset++){
            if (bitmap_get(&bitmap, index + offset) == true) { // Not contiguous freed memory
                broken_memory_flag = 1;
                index = index + offset; // To prevent looping through the same chunks that aren't available
                break; // Breaks offset loop
            }
        }

        if(broken_memory_flag) continue; // continue on to next memory chunk (in this case it is the index + offset one)

        for (int i = 0; i < block_count; i++) {
            bitmap_set(&bitmap, index + i, true);
        }
        kheap->chunk.used += size;
        return (void *) (kheap->chunk.start + index * kheap->chunk.granularity);
    }
    
    PANIC("Your heap ran out of space or screwed up real bad, USED: %d, AVAILABLE%d", kheap->chunk.used, kheap->chunk.size);
    return NULL;
}

void kfree(void *ptr, size_t size) {
    bitmap_t bitmap = kheap->bitmap;
    size_t block_count = KHEAP_BITMAP_SIZE(size, kheap->chunk.granularity);
    
    int index = ((int)ptr - (int)kheap->chunk.start) / kheap->chunk.granularity;
    
    for (int i = 0; i < block_count; i++) {
        bitmap_set(&bitmap, index + i, false);
    }
    kheap->chunk.used -= size;
}