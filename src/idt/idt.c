#include "idt/idt.h"

#include "lib/kstdio.h"
#include "gdt/gdt.h"

#define IDT_ENTRIES_AMOUNT 256

static idt_entry_t idt_entries[IDT_ENTRIES_AMOUNT];
static idt_ptr_t idt_ptr;

extern void* isr_stub_table[];

void idt_initialize() {
    idt_ptr = idt_create_ptr(IDT_ENTRIES_AMOUNT, (uint32_t) &idt_entries);

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_entries[vector] = idt_create_entry((uint32_t) isr_stub_table[vector], 0x8E);
    }

    idt_load(&idt_ptr);
}

void isr_handler(isr_frame_t *frame) {
    static unsigned char* exception_messages[32] = {
        "Division By Zero",
        "Debug",
        "Non Maskable Interrupt",
        "Breakpoint",
        "Into Detected Overflow",
        "Out of Bounds",
        "Invalid Opcode",
        "No Coprocessor",
        "Double fault",
        "Coprocessor Segment Overrun",
        "Bad TSS",
        "Segment not present",
        "Stack fault",
        "General protection fault",
        "Page fault",
        "Unknown Interrupt",
        "Coprocessor Fault",
        "Alignment Fault",
        "Machine Check",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved"
    };

    unsigned char* message = "Unknown Interrupt";
    if (frame->int_no < 32) {
        message = exception_messages[frame->int_no];
    }

    kprintf("Interrupt: %d (%s)\n", frame->int_no, message);

    if (frame->int_no == 14) {
        kprintf("Page fault at address: %x\n", frame->cr2);
    }

    // ReSharper disable once CppDFAEndlessLoop
    while (1) {
        __asm__ volatile("cli; hlt");
    }
}

idt_entry_t idt_create_entry(const uint32_t isr, uint8_t attributes) {
    idt_entry_t entry;

    entry.isr_low = (isr & 0x0000FFFF);
    entry.selector = GDT_KERNEL_CODE_SELECTOR;
    entry.zero = 0;
    entry.attributes = attributes;
    entry.isr_high = (isr & 0xFFFF0000) >> 16;

    return entry;
}

idt_ptr_t idt_create_ptr(const uint16_t entries, const uint32_t base) {
    idt_ptr_t ptr;

    ptr.limit = (entries * sizeof(idt_entry_t)) - 1;
    ptr.base = base;

    return ptr;
}
