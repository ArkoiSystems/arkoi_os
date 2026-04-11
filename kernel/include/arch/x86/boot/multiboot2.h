#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include <stdint.h>

// Multiboot2 tag types
#define MULTIBOOT2_TAG_ALIGN 8
#define MULTIBOOT2_TAG_TYPE_END 0
#define MULTIBOOT2_TAG_TYPE_COMMAND_LINE 1
#define MULTIBOOT2_TAG_TYPE_BOOT_LOADER_NAME 2
#define MULTIBOOT2_TAG_TYPE_MODULE 3
#define MULTIBOOT2_TAG_TYPE_MEMORY_MAP 6
#define MULTIBOOT2_TAG_TYPE_FRAMEBUFFER 8

// Memory region types for the memory map entries
#define MULTIBOOT2_MEMORY_AVAILABLE 1
#define MULTIBOOT2_MEMORY_RESERVED 2
#define MULTIBOOT2_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT2_MEMORY_NVS 4
#define MULTIBOOT2_MEMORY_BADRAM 5

/**
 * @brief Structure to represent a generic tag in the Multiboot2 information structure.
 */
typedef struct {
    uint32_t type; /**< Tag type (see MULTIBOOT2_TAG_TYPE_*) */
    uint32_t size; /**< Size of the tag, including the header and the data that follows */
} __attribute__((packed)) multiboot2_tag_t;

/**
 * @brief Structure to represent the command line tag provided by the Multiboot2 bootloader.
 */
typedef struct {
    uint32_t type;       /**< Tag type (MULTIBOOT2_TAG_TYPE_COMMAND_LINE) */
    uint32_t size;       /**< Size of the tag, including the header and the command line string */
    char command_line[]; /**< Command line string passed by the bootloader (null-terminated) */
} __attribute__((packed)) multiboot2_tag_boot_command_line_t;

/**
 * @brief Structure to represent the bootloader name tag provided by the Multiboot2 bootloader.
 */
typedef struct {
    uint32_t type; /**< Tag type (MULTIBOOT2_TAG_TYPE_BOOT_LOADER_NAME) */
    uint32_t size; /**< Size of the tag, including the header and the bootloader name string */
    char name[];   /**< Bootloader name string (null-terminated) */
} __attribute__((packed)) multiboot2_tag_boot_loader_name_t;

/**
 * @brief Structure to represent a loaded module, as provided by the Multiboot2 bootloader.
 */
typedef struct {
    uint32_t type;       /**< Tag type (MULTIBOOT2_TAG_TYPE_MODULE) */
    uint32_t size;       /**< Size of the tag, including the header and the command line string */
    uint32_t mod_start;  /**< Starting address of the module */
    uint32_t mod_end;    /**< Ending address of the module */
    char command_line[]; /**< Command line string associated with the module (null-terminated) */
} __attribute__((packed)) multiboot2_tag_module_t;

/**
 * @brief Structure to represent a memory map entry provided by the Multiboot2 bootloader.
 */
typedef struct {
    uint64_t base_address; /**< Starting address of the memory region */
    uint64_t length;       /**< Length of the memory region in bytes */
    uint32_t type;         /**< Type of memory region (1 for available, 2 for reserved, see MULTIBOOT2_MEMORY_*) */
    uint32_t reserved;     /**< Reserved field, should be set to 0 */
} __attribute__((packed)) multiboot2_memory_map_entry_t;

/**
 * @brief Structure to represent the memory map tag provided by the Multiboot2 bootloader.
 */
typedef struct {
    uint32_t type;                           /**< Tag type (MULTIBOOT2_TAG_TYPE_MEMORY_MAP) */
    uint32_t size;                           /**< Size of the tag, including the header and the memory map entries */
    uint32_t entry_size;                     /**< Size of each entry in the memory map */
    uint32_t entry_version;                  /**< Should be 0 for the current version of the Multiboot2 specification */
    multiboot2_memory_map_entry_t entries[]; /**< Flexible array member for memory map entries */
} __attribute__((packed)) multiboot2_tag_memory_map_t;

/**
 * @brief Structure to represent a color in the framebuffer.
 */
typedef struct {
    uint8_t red;   /**< Red color component (0-255) */
    uint8_t green; /**< Green color component (0-255) */
    uint8_t blue;  /**< Blue color component (0-255) */
} __attribute__((packed)) multiboot2_framebuffer_color_t;

/**
 * @brief Structure to represent the color information for an indexed color framebuffer.
 */
typedef struct {
    uint32_t num_colors;                     /**< Number of colors in the color palette */
    multiboot2_framebuffer_color_t colors[]; /**< Array of color entries in the color palette */
} __attribute__((packed)) multiboot2_framebuffer_indexed_t;

/**
 * @brief Structure to represent the color information for a direct color framebuffer.
 */
typedef struct {
    uint8_t red_field_position;   /**< Bit position of the red color field in a pixel */
    uint8_t red_mask_size;        /**< Number of bits used for the red color field */
    uint8_t green_field_position; /**< Bit position of the green color field in a pixel */
    uint8_t green_mask_size;      /**< Number of bits used for the green color field */
    uint8_t blue_field_position;  /**< Bit position of the blue color field in a pixel */
    uint8_t blue_mask_size;       /**< Number of bits used for the blue color field */
} __attribute__((packed)) multiboot2_framebuffer_direct_t;

/**
 * @brief Union to represent the color information for a framebuffer.
 */
typedef union {
    multiboot2_framebuffer_indexed_t indexed; /**< Color information for indexed color framebuffers */
    multiboot2_framebuffer_direct_t direct;   /**< Color information for direct color framebuffers */
} __attribute__((packed)) multiboot2_framebuffer_color_info_t;

/**
 * @brief Represents the framebuffer information tag provided by the Multiboot2 bootloader.
 */
typedef struct {
    uint32_t type;            /**< Tag type (MULTIBOOT2_TAG_TYPE_FRAMEBUFFER) */
    uint32_t size;            /**< Size of the tag, including the header and the framebuffer information */
    uint64_t address;         /**< Physical address of the framebuffer */
    uint32_t pitch;           /**< Number of bytes per horizontal line (including padding) */
    uint32_t width;           /**< Width of the framebuffer in pixels */
    uint32_t height;          /**< Height of the framebuffer in pixels */
    uint8_t bpp;              /**< Bits per pixel */
    uint8_t framebuffer_type; /**< Type of the framebuffer (0 for indexed color, 1 for direct color) */
    uint16_t reserved;        /**< Reserved field, should be set to 0 */
    multiboot2_framebuffer_color_info_t color_info; /**< Color information for the framebuffer */
} __attribute__((packed)) multiboot2_tag_framebuffer_t;

/**
 * @brief Represents the Multiboot2 information structure provided by the bootloader.
 *
 * This structure contains a header with the total size of the information structure and a reserved field, followed by a
 * flexible array of tags that provide various pieces of information about the boot environment, such as the command
 * line, bootloader name, memory map, and loaded modules. Each tag has a type and size, allowing for easy parsing of the
 * information provided by the bootloader.
 */
typedef struct {
    uint32_t total_size;     /**< Total size of the Multiboot2 information structure, including all tags */
    uint32_t reserved;       /**< Reserved field, should be set to 0 */
    multiboot2_tag_t tags[]; /**< Flexible array member for the tags that follow the header */
} __attribute__((packed)) multiboot2_info_t;

/**
 * @brief Structure to represent a memory region, as provided by the Multiboot2 information structure.
 */
typedef struct boot_memory_region {
    uint64_t base_address;           /**< Starting address of the memory region */
    uint64_t length;                 /**< Length of the memory region in bytes */
    struct boot_memory_region* next; /**< Pointer to the next memory region in the linked list */
} boot_memory_region_t;

/**
 * @brief Structure to represent a loaded module in the system, as provided by the Multiboot2 information structure.
 */
typedef struct boot_module {
    uint32_t mod_start;       /**< Start address of the module */
    uint32_t mod_end;         /**< End address of the module */
    char* command_line;       /**< Command line associated with the module */
    struct boot_module* next; /**< Pointer to the next module in the linked list */
} boot_module_t;

/**
 * @brief Structure to hold parsed boot information from the Multiboot2 specification.
 */
typedef struct {
    char* name;                             /**< Bootloader name */
    char* command_line;                     /**< Command line passed by the bootloader */
    boot_memory_region_t* ram_regions;      /**< Linked list of available RAM regions */
    boot_memory_region_t* reserved_regions; /**< Linked list of reserved memory regions */
    boot_module_t* modules;                 /**< Linked list of loaded modules */
} boot_info_t;

/**
 * @brief Parses the Multiboot2 information structure into a more convenient `boot_info_t` structure.
 *
 * This function extracts relevant information from the `multiboot2_info_t` structure, such as the bootloader name,
 * command line, memory map, and loaded modules, and organizes it into a more user-friendly `boot_info_t` structure. It
 * iterates through the linked list of tags in the Multiboot2 information structure and populates the corresponding
 * fields in the `boot_info_t` structure based on the type of each tag. The function also handles memory allocation for
 * the linked lists of memory regions and modules, ensuring that the parsed information is stored in a structured format
 * for easier access and use in the kernel.
 *
 * @param mb2_info  A pointer to the `multiboot2_info_t` structure provided by the bootloader, containing the raw
 *                  Multiboot2 information.
 * @param boot_info A pointer to the `boot_info_t` structure that will be populated with the parsed boot information.
 */
void multiboot2_parse_boot_info(const multiboot2_info_t* mb2_info, boot_info_t* boot_info);

/**
 * @brief Prints the contents of the `boot_info_t` structure in a human-readable format.
 *
 * This function is just a helper function to display the parsed boot information, including the bootloader name,
 * command line, memory regions, and loaded modules. It iterates through the linked lists of memory regions and modules
 * to print their details.
 *
 * @param boot_info A pointer to the `boot_info_t` structure containing the parsed boot information to be printed.
 */
void multiboot2_print_boot_info(const boot_info_t* boot_info);

/**
 * @brief Calculates the total size of the memory map.
 *
 * This function iterates through the linked list of boot memory regions and sums up the lengths of all the regions to
 * calculate the total size of the memory map.
 *
 * @param regions A pointer to the head of the linked list of `boot_memory_region_t`.
 * @return uint64_t The total size of the memory map.
 */
uint64_t multiboot2_memory_map_size(const boot_memory_region_t* regions);

/**
 * @brief Counts the number of memory regions in the linked list of boot memory regions.
 *
 * This function iterates through the linked list of boot memory regions and counts how many regions are present.
 *
 * @param regions A pointer to the head of the linked list of `boot_memory_region_t`.
 * @return uint32_t The number of memory regions in the linked list.
 */
uint32_t multiboot2_memory_map_count(const boot_memory_region_t* regions);

/**
 * @brief Counts the number of modules in the linked list of boot modules.
 *
 * This function iterates through the linked list of boot modules and counts how many modules are present.
 *
 * @param modules A pointer to the head of the linked list of `boot_module_t`.
 * @return uint32_t The number of modules in the linked list.
 */
uint32_t multiboot2_module_count(const boot_module_t* modules);

#endif // MULTIBOOT2_H
