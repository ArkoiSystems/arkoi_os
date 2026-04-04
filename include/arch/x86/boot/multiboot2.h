#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include <stdint.h>

#define MULTIBOOT2_TAG_END 0
#define MULTIBOOT2_TAG_BOOT_COMMAND 1
#define MULTIBOOT2_TAG_BOOT_LOADER_NAME 2
#define MULTIBOOT2_TAG_MODULES 3
#define MULTIBOOT2_TAG_BOOT_DEVICE 5

#define MULTIBOOT2_TAG_MEMORY_MAP 6
#define MULTIBOOT2_MEMORY_MAP_RAM 1

#define BOOT_MAX_MEMORY_REGIONS 8

typedef struct {
    uint32_t type;
    uint32_t size;
} __attribute__((packed)) multiboot2_tag_t;

typedef struct {
    uint32_t type;
    uint32_t size;
    char command_line[];
} __attribute__((packed)) multiboot2_tag_boot_command_t;

typedef struct {
    uint32_t type;
    uint32_t size;
    char boot_loader_name[];
} __attribute__((packed)) multiboot2_tag_boot_loader_name_t;

typedef struct {
    uint32_t type;
    uint32_t size;
    uint32_t mod_start;
    uint32_t mod_end;
    char cmdline[];
} __attribute__((packed)) multiboot2_tag_modules_t;

typedef struct {
    uint32_t type;
    uint32_t size;
    uint32_t biosdev;
    uint32_t partition;
    uint32_t sub_partition;
} __attribute__((packed)) multiboot2_tag_boot_device_t;

typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} __attribute__((packed)) multiboot2_memory_map_entry_t;

typedef struct {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    multiboot2_memory_map_entry_t entries[];
} __attribute__((packed)) multiboot2_tag_memory_map_t;

typedef struct {
    uint32_t total_size;
    uint32_t reserved;
    multiboot2_tag_t tags[];
} __attribute__((packed)) multiboot2_info_t;

typedef struct {
    uint64_t base_addr;
    uint64_t length;
} boot_memory_region_t;

typedef struct {
    boot_memory_region_t ram_regions[BOOT_MAX_MEMORY_REGIONS];
    uint32_t ram_regions_count;
} boot_info_t;

void multiboot2_parse_boot_info(const multiboot2_info_t* mb2_info, boot_info_t* boot_info);

void multiboot2_ram_size(const boot_info_t* boot_info, uint64_t* total_ram_size);

#endif // MULTIBOOT2_H
