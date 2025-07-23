#include "drivers/pit.h"

#include "idt/pic.h"

volatile uint32_t ticks = 0;

void pit_initialize() {
    irq_install(0, &pit_handler);
    pic_clear_mask(0);
}

void pit_handler(const isr_frame_t *) {
    ticks++;
}
