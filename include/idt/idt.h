#ifndef IDT_H
#define IDT_H

#include <stdint.h>

void idt_initialize();

typedef struct {
    // Pushed manually in isr_common_stub
    uint32_t cr2, ds;

    // Pushed by the pusha instruction
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;

    // Pushed manually by the stub (e.g., isr_stub_0)
    uint32_t int_no, err_code;

    // Pushed automatically by the CPU
    uint32_t eip, cs, eflags;

    // Pushed if the ISR came from user mode (Ring 3)
    uint32_t user_esp, ss;
} __attribute__((packed)) isr_frame_t;

void isr_handler(isr_frame_t *frame);

typedef struct {
    uint16_t isr_low;    // The lower 16bit of the ISRs address
    uint16_t selector;   // The GDT segment selector that the CPU will load into CS before calling the ISR
    uint8_t  zero;       // Some reserved bits that are always set to zero
    uint8_t  attributes; // Types and attributes
    uint16_t isr_high;   // The higher 16bit of the ISRs address
} __attribute__((packed)) idt_entry_t;

idt_entry_t idt_create_entry(uint32_t isr, uint8_t attributes);

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

idt_ptr_t idt_create_ptr(uint16_t entries, uint32_t base);

extern void idt_load(idt_ptr_t *idt_ptr);

#endif //IDT_H
