#include "drivers/pit.h"
#include <stdint.h>

#include "arch/x86/idt/pic.h"
#include "lib/kio.h"
#include "lib/kstdio.h"

#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_DATA_PORT 0x40
#define PIT_CHANNEL0_LOHI_MODE3 0x36 // 00110110 (CNTRL: 00=channel0, RW=11=lo/hi, MODE=110=square wave, BCD=0)
#define PIT_BASE_FREQUENCY 1193182

#define PIT_FREQUENCY 1000 // 1000 Hz (1 ms per tick)

volatile uint32_t ticks = 0;

static void pit_set_frequency(uint32_t frequency) {
    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;
    outb(PIT_COMMAND_PORT, PIT_CHANNEL0_LOHI_MODE3);
    outb(PIT_CHANNEL0_DATA_PORT, divisor & 0xFF);
    outb(PIT_CHANNEL0_DATA_PORT, (divisor >> 8) & 0xFF);
}

void pit_init() {
    irq_install(0, &pit_handler);
    pic_clear_mask(0);

    pit_set_frequency(PIT_FREQUENCY);
}

void pit_handler([[maybe_unused]] const isr_frame_t* frame) {
    ticks++;
}

void pit_wait(uint32_t milliseconds) {
    uint32_t target_ticks = ticks + (milliseconds * PIT_FREQUENCY / 1000);
    while (ticks < target_ticks);
}
