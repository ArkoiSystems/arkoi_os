#include "idt/idt.h"

#include "lib/kstdio.h"
#include "gdt/gdt.h"
#include "idt/pic.h"
#include "lib/kpanic.h"

#define IDT_ENTRIES_AMOUNT 48

static idt_entry_t idt_entries[IDT_ENTRIES_AMOUNT];
static idt_ptr_t idt_ptr;

extern void* isr_stub_table[];
static isr_t isr_handlers[32] = {0};

extern void* irq_stub_table[];
static irq_t irq_handlers[16] = {0};

void idt_initialize() {
    idt_ptr = idt_create_ptr(IDT_ENTRIES_AMOUNT, (uint32_t) &idt_entries);

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_entries[vector] = idt_create_entry((uint32_t) isr_stub_table[vector], 0x8E);
    }

    pic_remap(0x20, 0x28);
    for (uint8_t vector = 32; vector < 48; vector++) {
        idt_entries[vector] = idt_create_entry((uint32_t) irq_stub_table[vector - 32], 0x8E);
    }

    idt_load(&idt_ptr);
}

void isr_install(const uint8_t isr, const isr_t handler) {
    isr_handlers[isr] = handler;
}

void isr_uninstall(const uint8_t isr) {
    isr_handlers[isr] = 0;
}

void isr_handler(const isr_frame_t *frame) {
    if (frame->int_no >= 32) PANIC("Invalid ISR with interrupt number %d", frame->int_no);

    static char* exception_messages[32] = {
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

    char *message = exception_messages[frame->int_no];
    kprintf("Interrupt: %d (%s)\n", frame->int_no, message);

    if (frame->int_no == 14) {
        kprintf("Page fault at address: %x\n", frame->cr2);
    }

    isr_t handler = isr_handlers[frame->int_no];
    if (handler) handler(frame);

    __asm__ volatile("cli; hlt" ::: "memory");
}

void irq_install(const uint8_t irq, const irq_t handler) {
    irq_handlers[irq] = handler;
}

void irq_uninstall(const uint8_t irq) {
    irq_handlers[irq] = 0;
}

void irq_handler(const isr_frame_t *frame) {
    const uint32_t irq = frame->int_no - 32;

    const irq_t handler = irq_handlers[irq];
    if (handler) handler(frame);

    pic_send_eoi(irq);
}

idt_entry_t idt_create_entry(const uint32_t isr, const uint8_t attributes) {
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
