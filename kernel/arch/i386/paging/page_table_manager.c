#include <kernel/paging/pfa.h>
#include <kernel/paging/paging.h>

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

page_directory_t *pd;
extern uint32_t KERNEL_END;

void ptm_initialize() {
    // Round up to nearest page boundary in order to keep everything 4kib aligned
    if (KERNEL_END % 4096)
        KERNEL_END = KERNEL_END + (4096 - KERNEL_END % 4096);

    // Initialize page directory at the end of kernel
    uint32_t pd_pages = NUM_PAGES(sizeof(page_directory_t));
    pd = (page_directory_t *) KERNEL_END;
    KERNEL_END += pd_pages*4096;
    memset(pd, 0, pd_pages*4096); // Unitialize everything
    pd->phys_addr = (uint32_t) pd;
    
    // Create all of the page tables at the end of the kernel
    for (uint32_t index = 0; index < 1024; index++) {
        uint32_t pde = pd->entries[index];
        if (!pde) {
            // No page directory entry
            // Create a page table for this entry to point to
            uint32_t page_table_pages = NUM_PAGES(sizeof(page_table_t));
            page_table_t *page_table = (page_table_t *) KERNEL_END;
            KERNEL_END += page_table_pages * 4096;
            memset(page_table, 0, page_table_pages*4096); // Unitialize everything

            pde = (uint32_t) page_table;
            pde |= 0x01; // Present
            // Page tables should be writable and only accessable to kernel
            pde |= 0x03; // If writable sets second to last LSB
            pde |= 0x0; // A zero on the 3rd bit means that only kernel can access

            pd->entries[index] = pde; // Update page directory entry
            pd->phys_entries[index] = pde;
        }
    }

    for (uint32_t addr = 0; addr < KERNEL_END; addr += 4096) {
        ptm_map_addr(addr, addr, true, true);
    }
    
    ptm_load(pd);
}

// Once we've mapped the majority of things out, there could be a problem with loading the correct physical addresses into CR3
void ptm_map_addr(uint32_t addr, uint32_t phys_addr, bool is_kernel, bool is_writable) {
    uint32_t pde_index = addr >> 22;
    uint32_t pde = pd->entries[pde_index];
    // Doesn't allow for re-mapping the address, so should be fixed later (maybe with overwrite flag?)
    if (!pde) {
        // Shouldn't happen because we already made all of the page directories
        printf("OOPSIES, %d", pde_index);
        abort();
        
        // // No page directory entry
        // // Create a page table for this entry to point to
        // uint32_t page_table_pages = NUM_PAGES(sizeof(page_table_t));
        // page_table_t *page_table = (page_table_t *) pfa_request_pages(page_table_pages);
        // memset(page_table, 0, page_table_pages*4096); // Unitialize everything

        // pde = (uint32_t) page_table;
        // pde |= 0x01; // Present
        // pde |= is_writable ? 0x03 : 0x0; // If writable sets second to last LSB
        // pde |= is_kernel ? 0x0 : 0x05; // A zero on the 3rd bit means that only kernel can access

        // pd->entries[pde_index] = pde; // Update page directory entry
        // pd->phys_entries[pde_index] = pde;
    }

    page_table_t *page_table = (page_table_t *)((pde >> 12) << 12);
    uint32_t pte_index = addr >> 12 & 0x03FF; // Keep middle 10 bits of address
    
    uint32_t pte = page_table->entries[pte_index];
    if(!pte) {
        pte = phys_addr;
        pte |= 0x01; // Present
        pte |= is_writable ? 0x03 : 0x0; // If writable sets second to last LSB
        pte |= is_kernel ? 0x0 : 0x05; // A zero on the 3rd bit means that only kernel can access

        page_table->entries[pte_index] = pte; // Update page table entry
    }
}

void ptm_load(page_directory_t *dir)
{
   asm volatile("mov %0, %%cr3":: "r"(&dir->phys_entries));
   uint32_t cr0;
   asm volatile("mov %%cr0, %0": "=r"(cr0));
   cr0 |= 0x80000000; // Enable paging!
   asm volatile("mov %0, %%cr0":: "r"(cr0));
}
