#include <kernel/tags.h>

#include <string.h>

// https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html
// typedef struct __attribute__((packed)) {
//     multiboot_tag_basic_meminfo *basic_meminfo;
//     multiboot_tag_bootdev       *bios_bootdev;
//     multiboot_tag_module        *modules;
//     multiboot_tag_elf_sections  *elf_symbols;
//     multiboot_tag_mmap          *memory_map;
//     multiboot_tag_old_acpi      *acpi_old;
//     multiboot_tag_new_acpi      *acpi_new;
// } multiboot_tags_info_t;

void load_tags(multiboot_tags_info_t *info, unsigned long addr) {
    // Clear all values
    memset((uint8_t *)info, 0, sizeof(multiboot_tags_info_t));
    
    struct multiboot_tag *tag;
    
    // https://www.gnu.org/software/grub/manual/multiboot2/html_node/kernel_002ec.html#kernel_002ec
    for (tag = (struct multiboot_tag *) (addr + 8);
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
                                       + ((tag->size + 7) & ~7)))
    {
        if (tag->type == MULTIBOOT_TAG_TYPE_BASIC_MEMINFO) {
            info->basic_meminfo = (struct multiboot_tag_basic_meminfo *)tag;
        }
        else if(tag->type == MULTIBOOT_TAG_TYPE_BOOTDEV) {
            info->bios_bootdev = (struct multiboot_tag_bootdev *)tag;
        }
        else if(tag->type == MULTIBOOT_TAG_TYPE_MODULE) {
            info->modules = (struct multiboot_tag_module *)tag;
        }
        else if(tag->type == MULTIBOOT_TAG_TYPE_ELF_SECTIONS) {
            info->elf_symbols = (struct multiboot_tag_elf_sections *)tag;
        }
        else if(tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
            info->memory_map = (struct multiboot_tag_mmap *)tag;
        }
        else if(tag->type == MULTIBOOT_TAG_TYPE_ACPI_OLD) {
            info->acpi_old = (struct multiboot_tag_old_acpi *)tag;
        }
        else if(tag->type == MULTIBOOT_TAG_TYPE_ACPI_NEW) {
            info->acpi_new = (struct multiboot_tag_new_acpi *)tag;
        }
    }
}