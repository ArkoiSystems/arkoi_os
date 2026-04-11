#include "drivers/keyboard.h"

#include <stdbool.h>
#include <stdint.h>

#include "arch/x86/idt/idt.h"
#include "arch/x86/idt/pic.h"
#include "lib/kbuffer.h"
#include "lib/kio.h"

#define KEYBOARD_BUFFER_SIZE 256

#define EXTENDED_SCANCODE_PREFIX 0xE0
#define RELEASED_SCANCODE_MASK 0x80

#define PS2_DATA_PORT 0x60

#define PS2_STATUS_PORT 0x64
#define PS2_STATUS_OUTPUT_BUFFER_FULL 0x01

static keyboard_event_t g_keyboard_events[KEYBOARD_BUFFER_SIZE];
static cyclic_buffer_t g_keyboard_buffer;

static bool g_extended_pending = false;

static bool g_caps_locked = false;
static bool g_shift_held = false;
static bool g_ctrl_held = false;
static bool g_alt_held = false;

void keyboard_init() {
    cyclic_buffer_init(&g_keyboard_buffer, g_keyboard_events, KEYBOARD_BUFFER_SIZE, sizeof(g_keyboard_events[0]));

    irq_install(1, &keyboard_handler);
    pic_clear_mask(1);
}

static void handle_scancode(const uint16_t raw_scancode, bool extended) {
    const bool is_released = (raw_scancode & RELEASED_SCANCODE_MASK) != 0;
    const uint8_t keycode = raw_scancode & ~RELEASED_SCANCODE_MASK;

    const uint16_t scancode_prefix = (extended ? EXTENDED_SCANCODE_PREFIX : 0x00) << 8;
    const uint16_t extended_scancode = scancode_prefix | keycode;

    const keyboard_scancode_t scancode = (keyboard_scancode_t)extended_scancode;
    if (scancode == SCANCODE_LEFT_SHIFT || scancode == SCANCODE_RIGHT_SHIFT) {
        g_shift_held = !is_released;
    } else if (scancode == SCANCODE_LEFT_CTRL || scancode == SCANCODE_RIGHT_CTRL) {
        g_ctrl_held = !is_released;
    } else if (scancode == SCANCODE_LEFT_ALT || scancode == SCANCODE_RIGHT_ALT) {
        g_alt_held = !is_released;
    } else if (scancode == SCANCODE_CAPS_LOCK && !is_released) {
        g_caps_locked = !g_caps_locked;
    }

    keyboard_event_t event = { .keycode = keycode,
                               .scancode = scancode,
                               .is_pressed = !is_released,
                               .shift_held = g_shift_held,
                               .ctrl_held = g_ctrl_held,
                               .alt_held = g_alt_held,
                               .caps_locked = g_caps_locked };

    cyclic_buffer_push(&g_keyboard_buffer, &event);
}

void keyboard_handler([[maybe_unused]] const isr_frame_t* frame) {
    while (inb(PS2_STATUS_PORT) & PS2_STATUS_OUTPUT_BUFFER_FULL) {
        const uint8_t scancode = inb(PS2_DATA_PORT);

        if (scancode == EXTENDED_SCANCODE_PREFIX) {
            g_extended_pending = true;
            return;
        }

        const bool extended = g_extended_pending;
        g_extended_pending = false;

        handle_scancode(scancode, extended);
    }
}

bool keyboard_has_event() {
    return !cyclic_buffer_is_empty(&g_keyboard_buffer);
}

bool keyboard_get_event(keyboard_event_t* event) {
    if (!keyboard_has_event()) {
        // TODO(tbd): Report error
        return false;
    }

    cyclic_buffer_pop(&g_keyboard_buffer, event);
    return true;
}

bool keyboard_scancode_to_ascii(const keyboard_event_t* event, char* ascii) {
    if (event->scancode >= 128) {
        return false;
    }

    if (event->shift_held || event->caps_locked) {
        *ascii = SCANCODE_TO_ASCII_SHIFT[event->scancode];
    } else {
        *ascii = SCANCODE_TO_ASCII[event->scancode];
    }

    return (*ascii != 0);
}
