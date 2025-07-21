#include "idt/pic.h"

#include "lib/kio.h"

void pic_remap(const uint8_t offset_1, const uint8_t offset_2) {
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); // Starts the initialization sequence (in cascade mode)
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, offset_1);                      // ICW2: Master PIC vector offset
    outb(PIC2_DATA, offset_2);                      // ICW2: Slave PIC vector offset
    outb(PIC1_DATA, 1 << CASCADE_IRQ);         // ICW3: Tell Master PIC that there is a slave PIC at IRQ2
    outb(PIC2_DATA, 2);                        // ICW3: Tell Slave PIC its cascade identity (0000 0010)
    outb(PIC1_DATA, ICW4_8086);                // ICW4: Have the PICs use 8086 mode (and not 8080 mode)
    outb(PIC2_DATA, ICW4_8086);

    // Unmask both PICs.
    outb(PIC1_DATA, 0);
    outb(PIC2_DATA, 0);
}

void pic_send_eoi(const uint8_t irq) {
    if (irq >= 8) outb(PIC2_COMMAND, PIC_EOI);

    outb(PIC1_COMMAND, PIC_EOI);
}

uint8_t pic_set_mask(uint8_t irq) {
    uint16_t port;
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }

    const uint8_t value = inb(port) | (1 << irq);
    outb(port, value);

    return value;
}

uint8_t pic_clear_mask(uint8_t irq) {
    uint16_t port;
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }

    const uint8_t value = inb(port) & ~(1 << irq);
    outb(port, value);

    return value;
}

void pic_disable() {
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}
