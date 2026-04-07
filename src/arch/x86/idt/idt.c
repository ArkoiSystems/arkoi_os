#include "arch/x86/idt/idt.h"

#include "arch/x86/gdt/gdt.h"
#include "arch/x86/idt/pic.h"
#include "lib/kpanic.h"
#include "lib/kstdio.h"

#define IDT_ENTRIES_AMOUNT 48

static idt_entry_t g_idt_entries[IDT_ENTRIES_AMOUNT];
static idt_ptr_t g_idt_ptr;

extern void* g_isr_stub_table[];
static isr_t g_isr_handlers[32] = { 0 };

extern void* g_irq_stub_table[];
static irq_t g_irq_handlers[16] = { 0 };

static const char* EXCEPTION_MESSAGES[32] = { [EXCEPTION_DIVIDE_BY_ZERO] = "Division By Zero",
                                              [EXCEPTION_DEBUG] = "Debug",
                                              [EXCEPTION_NON_MASKABLE_INTERRUPT] = "Non Maskable Interrupt",
                                              [EXCEPTION_BREAKPOINT] = "Breakpoint",
                                              [EXCEPTION_INTO_DETECTED_OVERFLOW] = "Into Detected Overflow",
                                              [EXCEPTION_OUT_OF_BOUNDS] = "Out of Bounds",
                                              [EXCEPTION_INVALID_OPCODE] = "Invalid Opcode",
                                              [EXCEPTION_NO_COPROCESSOR] = "No Coprocessor",
                                              [EXCEPTION_DOUBLE_FAULT] = "Double fault",
                                              [EXCEPTION_COPROCESSOR_SEGMENT_OVERRUN] = "Coprocessor Segment Overrun",
                                              [EXCEPTION_BAD_TSS] = "Bad TSS",
                                              [EXCEPTION_SEGMENT_NOT_PRESENT] = "Segment not present",
                                              [EXCEPTION_STACK_FAULT] = "Stack fault",
                                              [EXCEPTION_GENERAL_PROTECTION_FAULT] = "General protection fault",
                                              [EXCEPTION_PAGE_FAULT] = "Page fault",
                                              [EXCEPTION_UNKNOWN_INTERRUPT] = "Unknown Interrupt",
                                              [EXCEPTION_COPROCESSOR_FAULT] = "Coprocessor Fault",
                                              [EXCEPTION_ALIGNMENT_FAULT] = "Alignment Fault",
                                              [EXCEPTION_MACHINE_CHECK] = "Machine Check",
                                              [EXCEPTION_RESERVED_19] = "Reserved",
                                              [EXCEPTION_RESERVED_20] = "Reserved",
                                              [EXCEPTION_RESERVED_21] = "Reserved",
                                              [EXCEPTION_RESERVED_22] = "Reserved",
                                              [EXCEPTION_RESERVED_23] = "Reserved",
                                              [EXCEPTION_RESERVED_24] = "Reserved",
                                              [EXCEPTION_RESERVED_25] = "Reserved",
                                              [EXCEPTION_RESERVED_26] = "Reserved",
                                              [EXCEPTION_RESERVED_27] = "Reserved",
                                              [EXCEPTION_RESERVED_28] = "Reserved",
                                              [EXCEPTION_RESERVED_29] = "Reserved",
                                              [EXCEPTION_RESERVED_30] = "Reserved",
                                              [EXCEPTION_RESERVED_31] = "Reserved" };

void idt_init() {
    g_idt_ptr = idt_create_ptr(IDT_ENTRIES_AMOUNT, (uint32_t)&g_idt_entries);

    for (uint8_t vector = 0; vector < 32; vector++) {
        g_idt_entries[vector] = idt_create_entry((uint32_t)g_isr_stub_table[vector], 0x8E);
    }

    pic_remap(0x20, 0x28);
    for (uint8_t vector = 32; vector < 48; vector++) {
        g_idt_entries[vector] = idt_create_entry((uint32_t)g_irq_stub_table[vector - 32], 0x8E);
    }

    idt_load(&g_idt_ptr);
}

void isr_install(const uint8_t isr, const isr_t handler) {
    g_isr_handlers[isr] = handler;
}

void isr_uninstall(const uint8_t isr) {
    g_isr_handlers[isr] = 0;
}

void isr_handler(const isr_frame_t* frame) {
    if (frame->int_no >= 32) {
        KPANIC("Invalid ISR with interrupt number %d", frame->int_no);
    }

    isr_t handler = g_isr_handlers[frame->int_no];
    if (handler) {
        handler(frame);
    } else {
        isr_exception_t exception = frame->int_no;
        char* message = EXCEPTION_MESSAGES[exception];

        kprintf("Interrupt: %d (%s)\n", exception, message);
    }

    __asm__ volatile("cli; hlt" ::: "memory");
}

void irq_install(const uint8_t irq, const irq_t handler) {
    g_irq_handlers[irq] = handler;
}

void irq_uninstall(const uint8_t irq) {
    g_irq_handlers[irq] = 0;
}

void irq_handler(const isr_frame_t* frame) {
    const uint32_t irq = frame->int_no - 32;

    const irq_t handler = g_irq_handlers[irq];
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
