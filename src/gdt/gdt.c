#include "gdt/gdt.h"

#include "lib/kstdio.h"

static gdt_entry_t gdt_entries[GDT_SEGMENTS];
static gdt_ptr_t gdt_ptr;

void gdt_initialize() {
    gdt_ptr = gdt_create_ptr(GDT_SEGMENTS, (uint32_t) &gdt_entries);

    gdt_entries[0] = gdt_create_entry(0x00000000, 0x00000, 0);
    gdt_entries[1] = gdt_create_entry(0x00000000, 0xFFFFF, GDT_CODE_RING0 | GDT_GRAN_4KB | GDT_GRAN_32BIT);
    gdt_entries[2] = gdt_create_entry(0x00000000, 0xFFFFF, GDT_DATA_RING0 | GDT_GRAN_4KB | GDT_GRAN_32BIT);
    gdt_entries[3] = gdt_create_entry(0x00000000, 0xFFFFF, GDT_CODE_RING3 | GDT_GRAN_4KB | GDT_GRAN_32BIT);
    gdt_entries[4] = gdt_create_entry(0x00000000, 0xFFFFF, GDT_DATA_RING3 | GDT_GRAN_4KB | GDT_GRAN_32BIT);

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

gdt_ptr_t gdt_create_ptr(const uint16_t segments, const uint32_t base) {
    gdt_ptr_t ptr;

    ptr.limit = (segments * sizeof(gdt_entry_t)) - 1;
    ptr.base  = base;

    return ptr;
}
