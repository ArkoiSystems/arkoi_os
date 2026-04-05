#include "arch/x86/boot/multiboot2.h"

#include <stdint.h>

#include "lib/kmemory.h"
#include "lib/kpanic.h"
#include "lib/kstring.h"

#define MULTIBOOT2_ALIGN_UP(x) (((x) + 7) & ~7)

static void parse_memory_map(const multiboot2_tag_memory_map_t* mb2_tag, boot_info_t* boot_info) {
    uint8_t* current = (uint8_t*)mb2_tag->entries;
    uint8_t* end = (uint8_t*)mb2_tag + mb2_tag->size;

    while (current < end) {
        multiboot2_memory_map_entry_t* entry = (multiboot2_memory_map_entry_t*)current;

        uint64_t base = entry->base_address;
        uint64_t length = entry->length;
        uint32_t type = entry->type;

        if (type == MULTIBOOT2_MEMORY_AVAILABLE) {
            if (boot_info->ram.count >= BOOT_MAX_MEMORY_REGIONS) {
                KPANIC("Too many memory regions. Maximum supported is %d.", BOOT_MAX_MEMORY_REGIONS);
            }

            boot_info->ram.regions[boot_info->ram.count].base_address = base;
            boot_info->ram.regions[boot_info->ram.count].length = length;
            boot_info->ram.count++;
        } else if (type == MULTIBOOT2_MEMORY_RESERVED) {
            if (boot_info->reserved.count >= BOOT_MAX_MEMORY_REGIONS) {
                KPANIC("Too many reserved memory regions. Maximum supported is %d.", BOOT_MAX_MEMORY_REGIONS);
            }

            boot_info->reserved.regions[boot_info->reserved.count].base_address = base;
            boot_info->reserved.regions[boot_info->reserved.count].length = length;
            boot_info->reserved.count++;
        }

        current += mb2_tag->entry_size;
    }
}

static void parse_command_line(const multiboot2_tag_boot_command_line_t* mb2_tag, boot_info_t* boot_info) {
    size_t length = kstrlen(mb2_tag->command_line);
    if (length >= BOOT_MAX_COMMAND_LINE_LENGTH) {
        KPANIC("The command line is too long to fit in the boot info structure.", 0);
    }

    kmemcpy(boot_info->command_line, mb2_tag->command_line, length);
    boot_info->command_line[length] = '\0';
}

static void parse_module(const multiboot2_tag_module_t* mb2_tag, boot_info_t* boot_info) {
    if (boot_info->module_count >= BOOT_MAX_MODULES) {
        KPANIC("Too many modules loaded. Maximum supported is %d.", BOOT_MAX_MODULES);
    }

    boot_module_t* module = &boot_info->modules[boot_info->module_count++];
    module->mod_start = mb2_tag->mod_start;
    module->mod_end = mb2_tag->mod_end;

    size_t cmdline_length = kstrlen(mb2_tag->command_line);
    if (cmdline_length >= BOOT_MAX_COMMAND_LINE_LENGTH) {
        KPANIC("Module command line is too long to fit in the boot info structure.", 0);
    }

    kmemcpy(module->command_line, mb2_tag->command_line, cmdline_length);
    module->command_line[cmdline_length] = '\0';
}

static void parse_name(const multiboot2_tag_boot_loader_name_t* mb2_tag, boot_info_t* boot_info) {
    size_t length = kstrlen(mb2_tag->name);
    if (length >= BOOT_MAX_NAME_LENGTH) {
        KPANIC("Boot loader name is too long to fit in the boot info structure.", 0);
    }

    kmemcpy(boot_info->name, mb2_tag->name, length);
    boot_info->name[length] = '\0';
}

void multiboot2_parse_boot_info(const multiboot2_info_t* mb2_info, boot_info_t* boot_info) {
    kmemset(boot_info, 0, sizeof(boot_info_t));

    uint8_t* current = (uint8_t*)mb2_info->tags;
    uint8_t* end = (uint8_t*)mb2_info + mb2_info->total_size;

    while (current < end) {
        multiboot2_tag_t* tag = (multiboot2_tag_t*)current;
        if (tag->type == MULTIBOOT2_TAG_TYPE_END) {
            break;
        }

        switch (tag->type) {
            case MULTIBOOT2_TAG_TYPE_MEMORY_MAP: {
                parse_memory_map((multiboot2_tag_memory_map_t*)tag, boot_info);
                break;
            }
            case MULTIBOOT2_TAG_TYPE_COMMAND_LINE: {
                parse_command_line((multiboot2_tag_boot_command_line_t*)tag, boot_info);
                break;
            }
            case MULTIBOOT2_TAG_TYPE_MODULE: {
                parse_module((multiboot2_tag_module_t*)tag, boot_info);
                break;
            }
            case MULTIBOOT2_TAG_TYPE_BOOT_LOADER_NAME: {
                parse_name((multiboot2_tag_boot_loader_name_t*)tag, boot_info);
                break;
            }
        }

        current = (current + MULTIBOOT2_ALIGN_UP(tag->size));
    }
}

uint64_t multiboot2_memory_map_size(const boot_memory_map_t* memory_map) {
    uint64_t total_size = 0;

    for (uint32_t index = 0; index < memory_map->count; index++) {
        total_size += memory_map->regions[index].length;
    }

    return total_size;
}
