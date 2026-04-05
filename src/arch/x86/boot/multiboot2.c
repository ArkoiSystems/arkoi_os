#include "arch/x86/boot/multiboot2.h"

#include <stdint.h>

#include "lib/kmemory.h"
#include "lib/kpanic.h"
#include "lib/kstring.h"

static void parse_memory_map(const multiboot2_tag_memory_map_t* mb2_basic_info, boot_info_t* boot_info) {
    uint8_t* start = (uint8_t*)mb2_basic_info->entries;
    uint8_t* end = (uint8_t*)mb2_basic_info + mb2_basic_info->size;

    uint8_t* current = start;
    while (current < end) {
        multiboot2_memory_map_entry_t* entry = (multiboot2_memory_map_entry_t*)current;

        uint64_t base = entry->base_address;
        uint64_t length = entry->length;
        uint32_t type = entry->type;

        if (type == MULTIBOOT2_MEMORY_AVAILABLE) {
            boot_info->ram.regions[boot_info->ram.count].base_address = base;
            boot_info->ram.regions[boot_info->ram.count].length = length;
            boot_info->ram.count++;
        }

        current += mb2_basic_info->entry_size;
    }
}

static void parse_command_line(const multiboot2_tag_boot_command_line_t* cmdline_tag, boot_info_t* boot_info) {
    size_t length = kstrlen(cmdline_tag->command_line);
    if (length >= BOOT_MAX_COMMAND_LINE_LENGTH) {
        KPANIC("The command line is too long to fit in the boot info structure.", 0);
    }

    memcpy(boot_info->command_line, cmdline_tag->command_line, length);
    boot_info->command_line[length] = '\0';
}

static void parse_module(const multiboot2_tag_module_t* modules_tag, boot_info_t* boot_info) {
    if (boot_info->module_count >= BOOT_MAX_MODULES) {
        KPANIC("Too many modules loaded. Maximum supported is %d.", BOOT_MAX_MODULES);
    }

    boot_module_t* module = &boot_info->modules[boot_info->module_count++];
    module->mod_start = modules_tag->mod_start;
    module->mod_end = modules_tag->mod_end;

    size_t cmdline_length = kstrlen(modules_tag->command_line);
    if (cmdline_length >= BOOT_MAX_COMMAND_LINE_LENGTH) {
        KPANIC("Module command line is too long to fit in the boot info structure.", 0);
    }

    memcpy(module->command_line, modules_tag->command_line, cmdline_length);
    module->command_line[cmdline_length] = '\0';
}

static void parse_name(const multiboot2_tag_boot_loader_name_t* boot_loader_name_tag, boot_info_t* boot_info) {
    size_t length = kstrlen(boot_loader_name_tag->name);
    if (length >= BOOT_MAX_NAME_LENGTH) {
        KPANIC("Boot loader name is too long to fit in the boot info structure.", 0);
    }

    memcpy(boot_info->name, boot_loader_name_tag->name, length);
    boot_info->name[length] = '\0';
}

void multiboot2_parse_boot_info(const multiboot2_info_t* mb2_info, boot_info_t* boot_info) {
    multiboot2_tag_t* tag = (multiboot2_tag_t*)mb2_info->tags;

    while (tag->type != MULTIBOOT2_TAG_TYPE_END) {
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

        tag = (void*)((uint8_t*)tag + ((tag->size + 7) & ~7));
    }
}

uint64_t multiboot2_memory_map_size(const boot_memory_map_t* memory_map) {
    uint64_t total_size = 0;

    for (uint32_t index = 0; index < memory_map->count; index++) {
        total_size += memory_map->regions[index].length;
    }

    return total_size;
}
