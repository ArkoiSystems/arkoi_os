#include "arch/x86/boot/multiboot2.h"

#include <stdint.h>

static void parse_memory_map(const multiboot2_tag_memory_map_t* mb2_basic_info, boot_info_t* boot_info) {
    uint8_t* start = (uint8_t*)mb2_basic_info->entries;
    uint8_t* end = (uint8_t*)mb2_basic_info + mb2_basic_info->size;

    uint32_t ram_region_index = 0;

    uint8_t* current = start;
    while (current < end) {
        multiboot2_memory_map_entry_t* entry = (multiboot2_memory_map_entry_t*)current;

        uint64_t base = entry->base_addr;
        uint64_t length = entry->length;
        uint32_t type = entry->type;

        if (type == MULTIBOOT2_MEMORY_MAP_RAM) {
            boot_info->ram_regions[ram_region_index].base_addr = base;
            boot_info->ram_regions[ram_region_index].length = length;
            ram_region_index++;
        }

        current += mb2_basic_info->entry_size;
    }

    boot_info->ram_regions_count = ram_region_index;
}

void multiboot2_parse_boot_info(const multiboot2_info_t* mb2_info, boot_info_t* boot_info) {
    multiboot2_tag_t* tag = (multiboot2_tag_t*)mb2_info->tags;

    while (tag->type != MULTIBOOT2_TAG_END) {
        switch (tag->type) {
            case MULTIBOOT2_TAG_MEMORY_MAP: {
                parse_memory_map((multiboot2_tag_memory_map_t*)tag, boot_info);
                break;
            }
        }

        tag = (void*)((uint8_t*)tag + ((tag->size + 7) & ~7));
    }
}

void multiboot2_ram_size(const boot_info_t* boot_info, uint64_t* total_ram_size) {
    uint64_t total_size = 0;

    for (uint32_t index = 0; index < boot_info->ram_regions_count; index++) {
        total_size += boot_info->ram_regions[index].length;
    }

    *total_ram_size = total_size;
}
