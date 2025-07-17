#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#define GDT_GRANULARITY_1B   (0 << 15) // Sets the granularity to be in 1 Byte blocks
#define GDT_GRANULARITY_4KB  (1 << 15) // Sets the granularity to be in 4KiB blocks
#define GDT_16Bit            (0 << 14) // Sets the segment to be 16-bit protected
#define GDT_32Bit            (1 << 14) // Sets the segment to be 32-bit protected
#define GDT_AVAILABLE        (1 << 12) // A reserved bit for the OS
#define GDT_PRESENT          (1 << 7) // Sets the segment to be valid
#define GDT_PRIVILEGE(level) ((level & 0x03) << 5) // Sets the privilege level of the segment (0, 1, 2, 3)
#define GDT_SYSTEM           (0 << 4) // Sets the segment to be a system segment
#define GDT_CODE_DATA        (1 << 4) // Sets the segment to be either code or data
#define GDT_DATA             (0 << 3) // Sets the segment to be data
#define GDT_CODE             (1 << 3) // Sets the segment to be executable code
#define GDT_READ             (1 << 1) // Enables reading if: GDT_CODE_DATA | GDT_CODE
#define GDT_WRITE            (1 << 1) // Enables writing if: GDT_CODE_DATA | GDT_DATA
#define GDT_ACCESSED         (1 << 0) // Enables the access bit

#define GDT_CODE_RING0 (GDT_PRESENT | GDT_PRIVILEGE(0) | GDT_CODE_DATA | GDT_CODE | GDT_READ)
#define GDT_DATA_RING0 (GDT_PRESENT | GDT_PRIVILEGE(0) | GDT_CODE_DATA | GDT_DATA | GDT_WRITE)
#define GDT_CODE_RING3 (GDT_PRESENT | GDT_PRIVILEGE(3) | GDT_CODE_DATA | GDT_CODE | GDT_READ)
#define GDT_DATA_RING3 (GDT_PRESENT | GDT_PRIVILEGE(3) | GDT_CODE_DATA | GDT_DATA | GDT_WRITE)

typedef struct {
    uint16_t segment_limit;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed)) gdt_entry_t;

gdt_entry_t gdt_create_entry(uint32_t base, uint32_t limit, uint16_t flags);

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

void gdt_initialize();

extern void gdt_load(gdt_ptr_t *gdt_ptr);

#endif //GDT_H
