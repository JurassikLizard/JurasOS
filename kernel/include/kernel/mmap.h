#ifndef _MMAP_H
#define _MMAP_H 1

#include <kernel/multiboot.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void mmap_print(multiboot_info_t* mbt);

#ifdef __cplusplus
}
#endif

#endif