// Heavy use of https://wiki.osdev.org/User:Pancakes/BitmapHeapImplementation

#ifndef _TAGS_H
#define _TAGS_H 1

#include <stdint.h>
#include <kernel/multiboot2.h>

#ifdef __cplusplus
extern "C" {
#endif

// https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html
typedef struct __attribute__((packed)) {
    struct multiboot_tag_basic_meminfo *basic_meminfo;
    struct multiboot_tag_bootdev       *bios_bootdev;
    struct multiboot_tag_module        *modules;
    struct multiboot_tag_elf_sections  *elf_symbols;
    struct multiboot_tag_mmap          *memory_map;
    struct multiboot_tag_old_acpi      *acpi_old;
    struct multiboot_tag_new_acpi      *acpi_new;
} multiboot_tags_info_t;

void load_tags(multiboot_tags_info_t *info, unsigned long addr);

#ifdef __cplusplus
}
#endif

#endif