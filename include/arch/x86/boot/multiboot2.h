#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include <stdint.h>

#include "lib/kbuffer.h"

#define MULTIBOOT2_TAG_ALIGN 8
#define MULTIBOOT2_TAG_TYPE_END 0
#define MULTIBOOT2_TAG_TYPE_COMMAND_LINE 1
#define MULTIBOOT2_TAG_TYPE_BOOT_LOADER_NAME 2
#define MULTIBOOT2_TAG_TYPE_MODULE 3
#define MULTIBOOT2_TAG_TYPE_MEMORY_MAP 6

#define MULTIBOOT2_MEMORY_AVAILABLE 1
#define MULTIBOOT2_MEMORY_RESERVED 2
#define MULTIBOOT2_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT2_MEMORY_NVS 4
#define MULTIBOOT2_MEMORY_BADRAM 5

#define BOOT_MAX_NAME_LENGTH 256
#define BOOT_MAX_COMMAND_LINE_LENGTH 256
#define BOOT_MAX_MEMORY_REGIONS 8
#define BOOT_MAX_MODULES 8

typedef struct {
    uint32_t type;
    uint32_t size;
} __attribute__((packed)) multiboot2_tag_t;

typedef struct {
    uint32_t type;
    uint32_t size;
    char command_line[0];
} __attribute__((packed)) multiboot2_tag_boot_command_line_t;

typedef struct {
    uint32_t type;
    uint32_t size;
    char name[0];
} __attribute__((packed)) multiboot2_tag_boot_loader_name_t;

typedef struct {
    uint32_t type;
    uint32_t size;
    uint32_t mod_start;
    uint32_t mod_end;
    char command_line[0];
} __attribute__((packed)) multiboot2_tag_module_t;

typedef struct {
    uint64_t base_address;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} __attribute__((packed)) multiboot2_memory_map_entry_t;

typedef struct {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    multiboot2_memory_map_entry_t entries[0];
} __attribute__((packed)) multiboot2_tag_memory_map_t;

typedef struct {
    uint32_t total_size;
    uint32_t reserved;
    multiboot2_tag_t tags[0];
} __attribute__((packed)) multiboot2_info_t;

typedef struct {
    uint64_t base_address;
    uint64_t length;
} boot_memory_region_t;

typedef struct {
    boot_memory_region_t regions[BOOT_MAX_MEMORY_REGIONS];
    uint32_t count;
} boot_memory_map_t;

typedef struct {
    uint32_t mod_start;
    uint32_t mod_end;
    char command_line[BOOT_MAX_COMMAND_LINE_LENGTH];
} boot_module_t;

typedef struct {
    char name[BOOT_MAX_NAME_LENGTH];
    char command_line[BOOT_MAX_COMMAND_LINE_LENGTH];
    boot_memory_map_t ram;
    boot_module_t modules[BOOT_MAX_MODULES];
    uint32_t module_count;
} boot_info_t;

void multiboot2_parse_boot_info(const multiboot2_info_t* mb2_info, boot_info_t* boot_info);

uint64_t multiboot2_memory_map_size(const boot_memory_map_t* memory_map);

#endif // MULTIBOOT2_H
