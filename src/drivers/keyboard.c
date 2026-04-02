#include "drivers/keyboard.h"

#include "drivers/vga.h"
#include "arch/x86/idt.h"
#include "arch/x86/pic.h"
#include "lib/kbuffer.h"
#include "lib/kio.h"
#include <stdbool.h>
#include <stdint.h>

#define KEYBOARD_BUFFER_SIZE 256

#define EXTENDED_SCANCODE_PREFIX 0xE0
#define RELEASED_SCANCODE_MASK 0x80

#define PS2_DATA_PORT 0x60

#define PS2_STATUS_PORT 0x64
#define PS2_STATUS_OUTPUT_BUFFER_FULL 0x01

static keyboard_event_t KEYBOARD_EVENTS[KEYBOARD_BUFFER_SIZE];
static cyclic_buffer_t KEYBOARD_BUFFER;

static bool EXTENDED_PENDING = false;

static bool CAPS_LOCKED = false;
static bool SHIFT_HELD = false;
static bool CTRL_HELD = false;
static bool ALT_HELD = false;

void keyboard_initialize() {
    cyclic_buffer_init(&KEYBOARD_BUFFER, KEYBOARD_EVENTS, KEYBOARD_BUFFER_SIZE, sizeof(KEYBOARD_EVENTS[0]));

    irq_install(1, &keyboard_handler);
    pic_clear_mask(1);
}

static void handle_scancode(const uint16_t raw_scancode, bool extended) {
    const bool is_released = (raw_scancode & RELEASED_SCANCODE_MASK) != 0;
    const uint8_t keycode = raw_scancode & ~RELEASED_SCANCODE_MASK;

    const uint16_t scancode_prefix = (extended ? EXTENDED_SCANCODE_PREFIX : 0x00) << 8;
    const uint16_t extended_scancode = scancode_prefix | keycode;

    const keyboard_scancode_t scancode = (keyboard_scancode_t) extended_scancode;
    if (scancode == SCANCODE_LEFT_SHIFT || scancode == SCANCODE_RIGHT_SHIFT) {
        SHIFT_HELD = !is_released;
    } else if (scancode == SCANCODE_LEFT_CTRL || scancode == SCANCODE_RIGHT_CTRL) {
        CTRL_HELD = !is_released;
    } else if (scancode == SCANCODE_LEFT_ALT || scancode == SCANCODE_RIGHT_ALT) {
        ALT_HELD = !is_released;
    } else if (scancode == SCANCODE_CAPS_LOCK && !is_released) {
        CAPS_LOCKED = !CAPS_LOCKED;
    }

    keyboard_event_t event = {
        .keycode = keycode,
        .scancode = scancode,
        .is_pressed = !is_released,
        .shift_held = SHIFT_HELD,
        .ctrl_held = CTRL_HELD,
        .alt_held = ALT_HELD,
        .caps_locked = CAPS_LOCKED
    };

    cyclic_buffer_push(&KEYBOARD_BUFFER, &event);
}

void keyboard_handler([[maybe_unused]] const isr_frame_t *frame) {
    while (inb(PS2_STATUS_PORT) & PS2_STATUS_OUTPUT_BUFFER_FULL) {
        const uint8_t scancode = inb(PS2_DATA_PORT);

        if (scancode == EXTENDED_SCANCODE_PREFIX) {
            EXTENDED_PENDING = true;
            return;
        }

        const bool extended = EXTENDED_PENDING;
        EXTENDED_PENDING = false;

        handle_scancode(scancode, extended);
    }
}

bool keyboard_has_event() {
    return !cyclic_buffer_is_empty(&KEYBOARD_BUFFER);
}

void keyboard_get_event(keyboard_event_t *event) {
    if (!keyboard_has_event()) {
        // TODO(tbd): Report error
        return;
    }

    cyclic_buffer_pop(&KEYBOARD_BUFFER, event);
}

size_t keyboard_scancode_to_ascii(const keyboard_event_t *event, char *ascii) {
    if (event->scancode >= 128) {
        return 1;
    }

    if (event->shift_held || event->caps_locked) {
        *ascii = SCANCODE_TO_ASCII_SHIFT[event->scancode];
    } else {
        *ascii = SCANCODE_TO_ASCII[event->scancode];
    }

    return (*ascii == 0);
}