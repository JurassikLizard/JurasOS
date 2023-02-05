#ifndef _PAGING_H
#define _PAGING_H 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct page_table_struct
{
   uint32_t entries[1024];
} page_table_t;

typedef struct page_directory_struct
{
   /**
      Array of page directory entries with top 20 bits being pointers to page tables
   **/
   uint32_t entries[1024];
   /**
      Array of pointers to the pagetables above, but gives their *physical*
      location, for loading into the CR3 register.
   **/
   uint32_t phys_entries[1024];
   /**
      The physical address of tablesPhysical. This comes into play
      when we get our kernel heap allocated and the directory
      may be in a different location in virtual memory.
   **/
   uint32_t phys_addr;
} page_directory_t;

/**
  Sets up the environment, page directories etc and
  enables paging.
**/
void ptm_initialize();

// Maps addr to phys addr and gives 2 flags
void ptm_map_addr(uint32_t addr, uint32_t phys_addr, bool is_kernel, bool is_writable);

// Loads page_directory into CR3 register
void ptm_load(page_directory_t *dir);

#ifdef __cplusplus
}
#endif

#endif