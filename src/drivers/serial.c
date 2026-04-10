#include "drivers/serial.h"

#include <stdint.h>

#include "lib/kio.h"
#include "lib/kstring.h"
#include "lib/memory/emm.h"

static serial_port_config_t* g_serial_ports = NULL;

static serial_port_config_t** get_serial_slot(uint16_t port) {
    serial_port_config_t** slot = &g_serial_ports;

    while (*slot) {
        if ((*slot)->port == port) {
            return slot;
        }
        slot = &(*slot)->next;
    }

    return slot;
}

static serial_port_config_t* get_serial_port(uint16_t port) {
    return *get_serial_slot(port);
}

static bool set_serial_config(uint16_t port, uint32_t baudrate) {
    serial_port_config_t** slot = get_serial_slot(port);
    serial_port_config_t* config = *slot;

    if (config == NULL) {
        config = (serial_port_config_t*)emm_alloc(sizeof(serial_port_config_t));
        if (!config) {
            return false;
        }

        config->next = NULL;
        *slot = config;
    }

    config->port = port;
    config->baudrate = baudrate;

    return true;
}

static bool serial_received(uint16_t port) {
    return inb(port + SERIAL_REG_LSR) & SERIAL_LSR_DATA_READY;
}

static bool read_char(uint16_t port, char* character) {
    while (!serial_received(port));

    *character = inb(port + SERIAL_REG_DATA);
    return true;
}

static void flush_fifo(uint16_t port) {
    while (serial_received(port)) {
        inb(port + SERIAL_REG_DATA);
    }
}

static bool is_transmit_empty(uint16_t port) {
    return inb(port + SERIAL_REG_LSR) & SERIAL_LSR_THR_EMPTY;
}

static bool write_char(uint16_t port, char character) {
    while (!is_transmit_empty(port));

    outb(port + SERIAL_REG_DATA, character);
    return true;
}

bool serial_init_port(
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
    flush_fifo(port);
    char received_char;
    write_char(port, 0x2A);
    read_char(port, &received_char);

    outb(port + SERIAL_REG_MCR, 0x0F); // Set normal operation mode

    // If the received character matches the sent character, we can assume the port is working correctly and save
    // the configuration.
    if ((received_char == 0x2A) && set_serial_config(port, baudrate)) {
        return true;
    }

    // If the test or configuration failed, reset the port to prevent issues.
    outb(port + SERIAL_REG_MCR, 0x00); // Reset MCR

    return false;
}

bool serial_read_char(uint16_t port, char* character) {
    if (character == NULL) {
        return false;
    }

    serial_port_config_t* config = get_serial_port(port);
    if (!config) {
        return false;
    }

    return read_char(config->port, character);
}

bool serial_read_string(uint16_t port, char* buffer, size_t buffer_size) {
    if (buffer == NULL || buffer_size == 0) {
        return false;
    }

    serial_port_config_t* config = get_serial_port(port);
    if (!config) {
        return false;
    }

    for (size_t index = 0; index < buffer_size - 1; index++) {
        if (!read_char(config->port, &buffer[index])) {
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

bool serial_write_char(uint16_t port, char character) {
    serial_port_config_t* config = get_serial_port(port);
    if (!config) {
        return false;
    }

    return write_char(config->port, character);
}

bool serial_write_string(uint16_t port, const char* text) {
    if (text == NULL) {
        return false;
    }

    serial_port_config_t* config = get_serial_port(port);
    if (!config) {
        return false;
    }

    size_t length = kstrlen(text);
    if (length == 0) {
        return true;
    }

    for (size_t index = 0; index < length; index++) {
        if (!write_char(config->port, text[index])) {
            return false;
        }
    }

    return true;
}
