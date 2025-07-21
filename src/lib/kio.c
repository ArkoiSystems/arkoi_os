#include "lib/kio.h"

void outb(uint16_t port, uint8_t data) {
    __asm__ volatile ("outb %b0, %w1" :: "a"(data), "Nd"(port) : "memory");
}

uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile ("inb %w1, %b0" : "=a"(result) : "Nd"(port) : "memory");
    return result;
}
