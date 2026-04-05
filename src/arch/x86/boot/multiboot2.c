#include "arch/x86/boot/multiboot2.h"

#include <stdint.h>

#include "lib/kmemory.h"
#include "lib/kpanic.h"
#include "lib/kstdio.h"
#include "lib/kstring.h"
#include "lib/memory/emm.h"

#define MULTIBOOT2_ALIGN_UP(x) (((x) + 7) & ~7)

static void parse_memory_map(const multiboot2_tag_memory_map_t* mb2_tag, boot_info_t* boot_info) {
    uint8_t* current = (uint8_t*)mb2_tag->entries;
    uint8_t* end = (uint8_t*)mb2_tag + mb2_tag->size;

    while (current < end) {
        multiboot2_memory_map_entry_t* entry = (multiboot2_memory_map_entry_t*)current;

        uint64_t base = entry->base_address;
        uint64_t length = entry->length;
        uint32_t type = entry->type;

        boot_memory_region_t* region = emm_alloc(sizeof(boot_memory_region_t));
        region->base_address = base;
        region->length = length;
        region->next = NULL;

        boot_memory_region_t** slot = NULL;
        if (type == MULTIBOOT2_MEMORY_AVAILABLE) {
            slot = &boot_info->ram_regions;
        } else if (type == MULTIBOOT2_MEMORY_RESERVED) {
            slot = &boot_info->reserved_regions;
        } else {
            // We currently ignore other types of memory regions, but we still need to advance the
            // pointer correctly.
            current += mb2_tag->entry_size;
            continue;
        }

        while (*slot != NULL) {
            slot = &(*slot)->next;
        }
        *slot = region;

        current += mb2_tag->entry_size;
    }
}

static void parse_command_line(const multiboot2_tag_boot_command_line_t* mb2_tag, boot_info_t* boot_info) {
    size_t length = kstrlen(mb2_tag->command_line);
    boot_info->command_line = emm_alloc(length + 1U);

    kmemcpy(boot_info->command_line, mb2_tag->command_line, length);
    boot_info->command_line[length] = '\0';
}

static void parse_module(const multiboot2_tag_module_t* mb2_tag, boot_info_t* boot_info) {
    boot_module_t* module = emm_alloc(sizeof(boot_module_t));
    module->mod_start = mb2_tag->mod_start;
    module->mod_end = mb2_tag->mod_end;
    module->next = NULL;

    boot_module_t** slot = &boot_info->modules;
    while (*slot != NULL) {
        slot = &(*slot)->next;
    }
    *slot = module;

    size_t cmdline_length = kstrlen(mb2_tag->command_line);
    module->command_line = emm_alloc(cmdline_length + 1U);

    kmemcpy(module->command_line, mb2_tag->command_line, cmdline_length);
    module->command_line[cmdline_length] = '\0';
}

static void parse_name(const multiboot2_tag_boot_loader_name_t* mb2_tag, boot_info_t* boot_info) {
    size_t name_length = kstrlen(mb2_tag->name);
    boot_info->name = emm_alloc(name_length + 1U);

    kmemcpy(boot_info->name, mb2_tag->name, name_length);
    boot_info->name[name_length] = '\0';
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

        current += MULTIBOOT2_ALIGN_UP(tag->size);
    }
}

void multiboot2_print_boot_info(const boot_info_t* boot_info) {
    kprintf("Booted using the \"%s\" boot loader\n", boot_info->name);
    kprintf("The command line is \"%s\"\n", boot_info->command_line);

    uint64_t ram_size_mb = multiboot2_memory_map_size(boot_info->ram_regions) / (1024 * 1024);
    uint32_t ram_count = multiboot2_memory_map_count(boot_info->ram_regions);
    kprintf("RAM has a size of %d MB with %d regions\n", (uint32_t)ram_size_mb, ram_count);

    boot_memory_region_t* current_ram = boot_info->ram_regions;
    while (current_ram != NULL) {
        uint64_t start_address = current_ram->base_address;
        uint64_t end_address = current_ram->base_address + current_ram->length;
        uint64_t size = current_ram->length / 1024;

        kprintf(" - RAM Region: %x - %x (%d KB)\n", (uint32_t)start_address, (uint32_t)end_address, (uint32_t)size);

        current_ram = current_ram->next;
    }

    uint64_t reserved_size_mb = multiboot2_memory_map_size(boot_info->reserved_regions) / 1024;
    uint32_t reserved_count = multiboot2_memory_map_count(boot_info->reserved_regions);
    kprintf("There are %d KB of reserved memory in %d regions\n", (uint32_t)reserved_size_mb, reserved_count);

    boot_memory_region_t* current_reserved = boot_info->reserved_regions;
    while (current_reserved != NULL) {
        uint64_t start_address = current_reserved->base_address;
        uint64_t end_address = current_reserved->base_address + current_reserved->length;
        uint64_t size = current_reserved->length / 1024;

        kprintf(
            " - Reserved Region: %x - %x (%d KB)\n", (uint32_t)start_address, (uint32_t)end_address, (uint32_t)size);

        current_reserved = current_reserved->next;
    }

    kprintf("There are %d modules loaded\n", multiboot2_module_count(boot_info->modules));

    boot_module_t* current_module = boot_info->modules;
    while (current_module != NULL) {
        uint32_t start_address = current_module->mod_start;
        uint32_t end_address = current_module->mod_end;
        uint32_t size = current_module->mod_end - current_module->mod_start;

        kprintf(" - Module: %x - %x (%d KB)\n", start_address, end_address, size / 1024);
        kprintf("   Command line: %s\n", current_module->command_line);

        current_module = current_module->next;
    }
}

uint64_t multiboot2_memory_map_size(const boot_memory_region_t* regions) {
    uint64_t total_size = 0;

    boot_memory_region_t* current = (boot_memory_region_t*)regions;
    while (current != NULL) {
        total_size += current->length;
        current = current->next;
    }

    return total_size;
}

uint32_t multiboot2_memory_map_count(const boot_memory_region_t* regions) {
    uint32_t count = 0;

    boot_memory_region_t* current = (boot_memory_region_t*)regions;
    while (current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}

uint32_t multiboot2_module_count(const boot_module_t* modules) {
    uint32_t count = 0;

    boot_module_t* current = (boot_module_t*)modules;
    while (current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}
