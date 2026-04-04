#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#define GDT_TO_SELECTOR_INDEX(index) (index << 3)
#define GDT_NULL_SELECTOR GDT_TO_SELECTOR_INDEX(0)
#define GDT_KERNEL_CODE_SELECTOR GDT_TO_SELECTOR_INDEX(1)
#define GDT_KERNEL_DATA_SELECTOR GDT_TO_SELECTOR_INDEX(2)
#define GDT_USER_CODE_SELECTOR GDT_TO_SELECTOR_INDEX(3)
#define GDT_USER_DATA_SELECTOR GDT_TO_SELECTOR_INDEX(4)
#define GDT_SEGMENTS 5

#define GDT_GRAN_1B (0 << 15)        // Sets the granularity to be in 1 Byte blocks
#define GDT_GRAN_4KB (1 << 15)       // Sets the granularity to be in 4KiB blocks
#define GDT_GRAN_16BIT (0 << 14)     // Sets the segment to be 16-bit protected
#define GDT_GRAN_32BIT (1 << 14)     // Sets the segment to be 32-bit protected
#define GDT_GRAN_AVAILABLE (1 << 12) // A reserved bit for the OS

#define GDT_ACCESS_PRESENT (1 << 7)                       // Sets the segment to be valid
#define GDT_ACCESS_PRIVILEGE(level) ((level & 0x03) << 5) // Sets the privilege level of the segment (0, 1, 2, 3)
#define GDT_ACCESS_SYSTEM (0 << 4)                        // Sets the segment to be a system segment
#define GDT_ACCESS_CODE_DATA (1 << 4)                     // Sets the segment to be either code or data
#define GDT_ACCESS_DATA (0 << 3)                          // Sets the segment to be data
#define GDT_ACCESS_CODE (1 << 3)                          // Sets the segment to be executable code
#define GDT_ACCESS_READ (1 << 1)                          // Enables reading if: GDT_CODE_DATA | GDT_CODE
#define GDT_ACCESS_WRITE (1 << 1)                         // Enables writing if: GDT_CODE_DATA | GDT_DATA
#define GDT_ACCESS_ACCESSED (1 << 0)                      // Enables the access bit

#define GDT_CODE_RING0                                                                                                 \
    (GDT_ACCESS_PRESENT | GDT_ACCESS_PRIVILEGE(0) | GDT_ACCESS_CODE_DATA | GDT_ACCESS_CODE | GDT_ACCESS_READ)
#define GDT_DATA_RING0                                                                                                 \
    (GDT_ACCESS_PRESENT | GDT_ACCESS_PRIVILEGE(0) | GDT_ACCESS_CODE_DATA | GDT_ACCESS_DATA | GDT_ACCESS_WRITE)
#define GDT_CODE_RING3                                                                                                 \
    (GDT_ACCESS_PRESENT | GDT_ACCESS_PRIVILEGE(3) | GDT_ACCESS_CODE_DATA | GDT_ACCESS_CODE | GDT_ACCESS_READ)
#define GDT_DATA_RING3                                                                                                 \
    (GDT_ACCESS_PRESENT | GDT_ACCESS_PRIVILEGE(3) | GDT_ACCESS_CODE_DATA | GDT_ACCESS_DATA | GDT_ACCESS_WRITE)

typedef struct {
    uint16_t segment_limit;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

/**
 * @brief Initializes the Global Descriptor Table (GDT) for the x86 architecture.
 *
 * Sets up and loads the processor's GDT entries required for protected-mode
 * segment handling (such as kernel code/data segments), preparing the CPU to
 * use the configured segment descriptors.
 *
 * @note This function is expected to be called early during kernel bootstrap
 *       before components that rely on proper segment configuration.
 */
void gdt_initialize();

/**
 * @brief Creates a Global Descriptor Table (GDT) entry.
 *
 * Constructs a GDT entry with the specified base address, limit, and flags.
 * GDT entries define memory segments and their access properties in x86 architectures.
 *
 * @param base  The base address of the memory segment.
 * @param limit The size limit of the memory segment.
 * @param flags The access and type flags for the segment (privilege level, type, granularity, etc.).
 *
 * @return `gdt_entry_t` A GDT entry structure containing the encoded segment descriptor.
 *
 * @note This function encodes the base, limit, and flags into the GDT entry format
 *       required by x86 processors.
 */
gdt_entry_t gdt_create_entry(uint32_t base, uint32_t limit, uint16_t flags);

/**
 * @brief Creates a Global Descriptor Table (GDT) pointer structure.
 *
 * Initializes and returns a `gdt_ptr_t` value using the provided segment count
 * and base address. This pointer is typically used when loading the GDT
 * (for example, with the `lgdt` instruction) during x86 setup.
 *
 * @param segments Number of GDT segments/descriptors represented by this table.
 * @param base     Linear memory address where the GDT begins.
 *
 * @return A populated `gdt_ptr_t` describing the GDT location and size.
 */
gdt_ptr_t gdt_create_ptr(uint16_t segments, uint32_t base);

/**
 * @brief Loads the Global Descriptor Table (GDT) into the processor.
 *
 * This function loads the GDT pointer into the processor's GDTR register,
 * making the specified GDT active for memory segmentation and protection.
 * This is typically called during kernel initialization to set up the
 * memory segmentation scheme for the operating system.
 *
 * @param gdt_ptr Pointer to a gdt_ptr_t structure containing the GDT base
 *                address and limit. Must not be NULL.
 *
 * @note This is an architecture-specific function for x86 processors.
 * @note Should only be called during kernel initialization or when
 *       switching GDT contexts.
 *
 * @see gdt_ptr_t
 */
extern void gdt_load(gdt_ptr_t* gdt_ptr);

#endif // GDT_H
