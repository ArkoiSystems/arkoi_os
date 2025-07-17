#include "gdt/gdt.h"

#define GDT_SEGMENTS 5

void gdt_initialize() {
    static gdt_entry_t gdt_entries[GDT_SEGMENTS];
    static gdt_ptr_t gdt_ptr;

    gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_SEGMENTS) - 1;
    gdt_ptr.base  = (uint32_t) &gdt_entries;

    gdt_entries[0] = gdt_create_entry(0x00000000, 0x00000, 0);
    gdt_entries[1] = gdt_create_entry(0x00000000, 0xFFFFF, GDT_CODE_RING0);
    gdt_entries[2] = gdt_create_entry(0x00000000, 0xFFFFF, GDT_DATA_RING0);
    gdt_entries[3] = gdt_create_entry(0x00000000, 0xFFFFF, GDT_CODE_RING3);
    gdt_entries[4] = gdt_create_entry(0x00000000, 0xFFFFF, GDT_DATA_RING3);

    gdt_load(&gdt_ptr);
}

gdt_entry_t gdt_create_entry(const uint32_t base, const uint32_t limit, const uint16_t flags) {
    gdt_entry_t entry;

    entry.segment_limit = (limit & 0x0000FFFF);
    entry.base_low      = (base  & 0x0000FFFF);
    entry.base_middle   = (base  & 0x00FF0000) >> 16;
    entry.access        = (flags & 0x00FF);
    entry.granularity   = (flags & 0xF000) >> 8
                        | (limit & 0x000F0000) >> 16;
    entry.base_high     = (base  & 0xFF000000) >> 24;

    return entry;
}
