#include "drivers/serial.h"

#include <stdint.h>

#include "lib/kio.h"
#include "lib/kstdio.h"
#include "lib/kstring.h"
#include "lib/memory/emm.h"

#define SERIAL_TEST_BYTE 0x2A

static serial_port_t* g_cached_lookup_result = NULL;
static serial_port_t* g_serial_ports = NULL;

static serial_port_t** get_serial_slot(uint16_t port) {
    serial_port_t** slot = &g_serial_ports;

    while (*slot) {
        if ((*slot)->port == port) {
            return slot;
        }
        slot = &(*slot)->next;
    }

    return slot;
}

static serial_port_t* upsert_serial_config(uint16_t port, uint32_t baudrate) {
    serial_port_t** slot = get_serial_slot(port);
    serial_port_t* config = *slot;

    if (config == NULL) {
        config = (serial_port_t*)emm_alloc(sizeof(serial_port_t));
        if (!config) {
            return NULL;
        }

        config->next = NULL;
        *slot = config;
    }

    config->port = port;
    config->baudrate = baudrate;

    return config;
}

static bool serial_received(uint16_t port) {
    return inb(port + SERIAL_REG_LSR) & SERIAL_LSR_DATA_READY;
}

static bool try_read_char(uint16_t port, char* character) {
    if (!serial_received(port)) {
        return false;
    }

    *character = inb(port + SERIAL_REG_DATA);
    return true;
}

static bool read_char(uint16_t port, char* character) {
    while (!try_read_char(port, character));
    return true;
}

static bool is_transmit_empty(uint16_t port) {
    return inb(port + SERIAL_REG_LSR) & SERIAL_LSR_THR_EMPTY;
}

static bool try_write_char(uint16_t port, char character) {
    if (!is_transmit_empty(port)) {
        return false;
    }

    outb(port + SERIAL_REG_DATA, character);
    return true;
}

static bool write_char(uint16_t port, char character) {
    while (!try_write_char(port, character));
    return true;
}

serial_port_t* serial_get_port(uint16_t port) {
    if (g_cached_lookup_result && g_cached_lookup_result->port == port) {
        return g_cached_lookup_result;
    }

    serial_port_t* result = *get_serial_slot(port);
    if (result) {
        g_cached_lookup_result = result;
    }

    return result;
}

void serial_init() {
    serial_init_port(SERIAL_PORT_COM1, 115200, SERIAL_DATA_BITS_8, SERIAL_PARITY_NONE, SERIAL_STOP_BITS_1);
    serial_init_port(SERIAL_PORT_COM2, 115200, SERIAL_DATA_BITS_8, SERIAL_PARITY_NONE, SERIAL_STOP_BITS_1);
}

serial_port_t* serial_init_port(
    uint16_t port, uint32_t baudrate, serial_data_bits_t data_bits, serial_parity_t parity,
    serial_stop_bits_t stop_bits) {
    uint16_t divisor = SERIAL_BASE_CLOCK / baudrate;

    outb(port + SERIAL_REG_IER, 0x00); // Disable all interrupts

    // Set baudrate using the calculated divisor
    outb(port + SERIAL_REG_LCR, 0x80); // Enable DLAB (set baud rate divisor)
    outb(port + SERIAL_REG_LSB, (uint8_t)(divisor & 0xFF));
    outb(port + SERIAL_REG_MSB, (uint8_t)((divisor >> 8) & 0xFF));
    outb(port + SERIAL_REG_LCR, 0x00); // Disable DLAB

    uint8_t lcr_reg = 0x00;
    lcr_reg |= (data_bits << SERIAL_LCR_DATA_SHIFT) & SERIAL_LCR_DATA_MASK;
    lcr_reg |= (stop_bits << SERIAL_LCR_STOP_SHIFT) & SERIAL_LCR_STOP_MASK;
    lcr_reg |= (parity << SERIAL_LCR_PARITY_SHIFT) & SERIAL_LCR_PARITY_MASK;
    outb(port + SERIAL_REG_LCR, lcr_reg);

    outb(port + SERIAL_REG_FCR, 0xC7); // Enable FIFO, clear receive and transmit buffers, set 14-byte threshold
    outb(port + SERIAL_REG_MCR, 0x0B); // IRQs enabled, RTS/DSR set
    outb(port + SERIAL_REG_MCR, 0x1E); // Set in loopback mode to test the serial chip

    // Test the serial port by sending a byte and reading it back
    char received_char;
    if (!try_write_char(port, SERIAL_TEST_BYTE)) {
        outb(port + SERIAL_REG_MCR, 0x00); // Reset MCR
        return NULL;
    }

    if (!try_read_char(port, &received_char)) {
        outb(port + SERIAL_REG_MCR, 0x00); // Reset MCR
        return NULL;
    }

    if (received_char != SERIAL_TEST_BYTE) {
        // If the test failed, reset the port to prevent issues.
        outb(port + SERIAL_REG_MCR, 0x00); // Reset MCR
        return NULL;
    }

    outb(port + SERIAL_REG_MCR, 0x0F); // Set normal operation mode

    // Now that the port is configured and tested, we can store the configuration for later use.
    serial_port_t* config = upsert_serial_config(port, baudrate);

    if (config == NULL) {
        // If the test failed, reset the port to prevent issues.
        outb(port + SERIAL_REG_MCR, 0x00); // Reset MCR
        return NULL;
    }

    return config;
}

bool serial_read_char(const serial_port_t* serial_port, char* character) {
    if (serial_port == NULL || character == NULL) {
        return false;
    }

    return read_char(serial_port->port, character);
}

bool serial_read_string(const serial_port_t* serial_port, char* buffer, size_t buffer_size) {
    if (serial_port == NULL || buffer == NULL || buffer_size == 0) {
        return false;
    }

    for (size_t index = 0; index < buffer_size - 1; index++) {
        if (!read_char(serial_port->port, &buffer[index])) {
            return false;
        }

        if (buffer[index] == '\n') {
            buffer[index + 1] = '\0';
            return true;
        }
    }

    buffer[buffer_size - 1] = '\0';
    return true;
}

bool serial_write_char(const serial_port_t* serial_port, char character) {
    if (serial_port == NULL) {
        return false;
    }

    return write_char(serial_port->port, character);
}

bool serial_write_string(const serial_port_t* serial_port, const char* text) {
    if (serial_port == NULL || text == NULL) {
        return false;
    }

    size_t length = kstrlen(text);
    if (length == 0) {
        return true;
    }

    for (size_t index = 0; index < length; index++) {
        if (!write_char(serial_port->port, text[index])) {
            return false;
        }
    }

    return true;
}
