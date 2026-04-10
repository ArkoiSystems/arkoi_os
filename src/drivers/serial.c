#include "drivers/serial.h"

#include <stdbool.h>
#include <stdint.h>

#include "arch/x86/idt/idt.h"
#include "arch/x86/idt/pic.h"
#include "lib/kbuffer.h"
#include "lib/kio.h"
#include "lib/kstring.h"

#define SERIAL_BUFFER_SIZE 1024

static bool g_com1_initialized = false;
static char g_com1_buffer_data[SERIAL_BUFFER_SIZE];
static cyclic_buffer_t g_com1_buffer;

static bool g_com2_initialized = false;
static char g_com2_buffer_data[SERIAL_BUFFER_SIZE];
static cyclic_buffer_t g_com2_buffer;

static bool serial_is_initialized(uint16_t port) {
    if (port == SERIAL_PORT_COM1) return g_com1_initialized;
    if (port == SERIAL_PORT_COM2) return g_com2_initialized;
    return false;
}

static bool serial_received(uint16_t port) {
    return inb(port + SERIAL_REG_LSR) & 1;
}

static char serial_receive(uint16_t port) {
    return inb(port);
}

static bool is_transmit_empty(uint16_t port) {
    return inb(port + SERIAL_REG_LSR) & 0x20;
}

static void com1_handler([[maybe_unused]] const isr_frame_t* frame) {
    while (serial_received(SERIAL_PORT_COM1)) {
        const char character = serial_receive(SERIAL_PORT_COM1);
        cyclic_buffer_push(&g_com1_buffer, (void*)&character);
    }
}

static void com2_handler([[maybe_unused]] const isr_frame_t* frame) {
    while (serial_received(SERIAL_PORT_COM2)) {
        const char character = serial_receive(SERIAL_PORT_COM2);
        cyclic_buffer_push(&g_com2_buffer, (void*)&character);
    }
}

bool serial_init_port(uint16_t port, serial_baud_t baud) {
    if (port != SERIAL_PORT_COM1 && port != SERIAL_PORT_COM2) {
        return false;
    }

    outb(port + SERIAL_REG_IER, 0x00); // Disable all interrupts
    outb(port + SERIAL_REG_LCR, 0x80); // Enable DLAB (set baud rate divisor)

    // Set baud rate divisor
    outb(port + SERIAL_REG_LSB, (uint8_t)(baud & 0xFF));
    outb(port + SERIAL_REG_MSB, (uint8_t)((baud >> 8) & 0xFF));

    outb(port + SERIAL_REG_LCR, 0x03); // 8 bits, no parity, one stop bit
    outb(port + SERIAL_REG_FCR, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(port + SERIAL_REG_MCR, 0x0B); // IRQs enabled, RTS/DSR set
    outb(port + SERIAL_REG_MCR, 0x1E); // Set in loopback mode to test the serial chip

    outb(port + 0, 0xAE);        // Test serial chip (send byte 0xAE and check if serial returns same byte)
    if (inb(port + 0) != 0xAE) { // Check if serial is faulty (i.e: not same byte as sent)
        return false;
    }

    outb(port + SERIAL_REG_MCR, 0x0F); // Set normal operation mode
    outb(port + SERIAL_REG_IER, 0x01); // Enable received data available interrupt

    cyclic_buffer_t* buffer = (port == SERIAL_PORT_COM1) ? &g_com1_buffer : &g_com2_buffer;
    char* buffer_data = (port == SERIAL_PORT_COM1) ? g_com1_buffer_data : g_com2_buffer_data;
    cyclic_buffer_init(buffer, buffer_data, SERIAL_BUFFER_SIZE, sizeof(char));

    uint8_t irq_id = (port == SERIAL_PORT_COM1) ? SERIAL_IRQ_COM1 : SERIAL_IRQ_COM2;
    irq_t irq_handler = (port == SERIAL_PORT_COM1) ? &com1_handler : &com2_handler;
    irq_install(irq_id, irq_handler);
    pic_clear_mask(irq_id);

    bool* initialized = (port == SERIAL_PORT_COM1) ? &g_com1_initialized : &g_com2_initialized;
    *initialized = true;

    return true;
}

bool serial_get_char(uint16_t port, char* character) {
    if (!serial_is_initialized(port)) {
        return false;
    }

    cyclic_buffer_t* buffer = (port == SERIAL_PORT_COM1) ? &g_com1_buffer : &g_com2_buffer;
    if (cyclic_buffer_is_empty(buffer)) {
        return false;
    }

    cyclic_buffer_pop(buffer, character);
    return true;
}

bool serial_write_char(uint16_t port, char character) {
    if (!serial_is_initialized(port)) {
        return false;
    }

    while (!is_transmit_empty(port));

    outb(port, character);
    return true;
}

bool serial_write_string(uint16_t port, const char* text) {
    if (!serial_is_initialized(port)) {
        return false;
    }

    for (size_t index = 0; index < kstrlen(text); index++) {
        if (!serial_write_char(port, text[index])) {
            return false;
        }
    }

    return true;
}
