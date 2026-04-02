#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "arch/x86/idt.h"
#include "lib/kbuffer.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    // Row 1
    SCANCODE_ESC            = 0x0001,
    SCANCODE_1              = 0x0002,
    SCANCODE_2              = 0x0003,
    SCANCODE_3              = 0x0004,
    SCANCODE_4              = 0x0005,
    SCANCODE_5              = 0x0006,
    SCANCODE_6              = 0x0007,
    SCANCODE_7              = 0x0008,
    SCANCODE_8              = 0x0009,
    SCANCODE_9              = 0x000A,
    SCANCODE_0              = 0x000B,
    SCANCODE_MINUS          = 0x000C,
    SCANCODE_EQUAL          = 0x000D,
    SCANCODE_BACKSPACE      = 0x000E,
    SCANCODE_TAB            = 0x000F,

    // Row 2
    SCANCODE_Q              = 0x0010,
    SCANCODE_W              = 0x0011,
    SCANCODE_E              = 0x0012,
    SCANCODE_R              = 0x0013,
    SCANCODE_T              = 0x0014,
    SCANCODE_Y              = 0x0015,
    SCANCODE_U              = 0x0016,
    SCANCODE_I              = 0x0017,
    SCANCODE_O              = 0x0018,
    SCANCODE_P              = 0x0019,
    SCANCODE_LBRACKET       = 0x001A,
    SCANCODE_RBRACKET       = 0x001B,
    SCANCODE_ENTER          = 0x001C,
    SCANCODE_LEFT_CTRL      = 0x001D,

    // Row 3
    SCANCODE_A              = 0x001E,
    SCANCODE_S              = 0x001F,
    SCANCODE_D              = 0x0020,
    SCANCODE_F              = 0x0021,
    SCANCODE_G              = 0x0022,
    SCANCODE_H              = 0x0023,
    SCANCODE_J              = 0x0024,
    SCANCODE_K              = 0x0025,
    SCANCODE_L              = 0x0026,
    SCANCODE_SEMICOLON      = 0x0027,
    SCANCODE_APOSTROPHE     = 0x0028,
    SCANCODE_GRAVE          = 0x0029,
    SCANCODE_LEFT_SHIFT     = 0x002A,
    SCANCODE_BACKSLASH      = 0x002B,

    // Row 4
    SCANCODE_Z              = 0x002C,
    SCANCODE_X              = 0x002D,
    SCANCODE_C              = 0x002E,
    SCANCODE_V              = 0x002F,
    SCANCODE_B              = 0x0030,
    SCANCODE_N              = 0x0031,
    SCANCODE_M              = 0x0032,
    SCANCODE_COMMA          = 0x0033,
    SCANCODE_DOT            = 0x0034,
    SCANCODE_SLASH          = 0x0035,
    SCANCODE_RIGHT_SHIFT    = 0x0036,
    SCANCODE_KP_STAR        = 0x0037,
    SCANCODE_LEFT_ALT       = 0x0038,
    SCANCODE_SPACE          = 0x0039,
    SCANCODE_CAPS_LOCK      = 0x003A,

    // Function keys
    SCANCODE_F1             = 0x003B,
    SCANCODE_F2             = 0x003C,
    SCANCODE_F3             = 0x003D,
    SCANCODE_F4             = 0x003E,
    SCANCODE_F5             = 0x003F,
    SCANCODE_F6             = 0x0040,
    SCANCODE_F7             = 0x0041,
    SCANCODE_F8             = 0x0042,
    SCANCODE_F9             = 0x0043,
    SCANCODE_F10            = 0x0044,

    SCANCODE_NUM_LOCK       = 0x0045,
    SCANCODE_SCROLL_LOCK    = 0x0046,

    // Keypad
    SCANCODE_KP_7           = 0x0047,
    SCANCODE_KP_8           = 0x0048,
    SCANCODE_KP_9           = 0x0049,
    SCANCODE_KP_MINUS       = 0x004A,
    SCANCODE_KP_4           = 0x004B,
    SCANCODE_KP_5           = 0x004C,
    SCANCODE_KP_6           = 0x004D,
    SCANCODE_KP_PLUS        = 0x004E,
    SCANCODE_KP_1           = 0x004F,
    SCANCODE_KP_2           = 0x0050,
    SCANCODE_KP_3           = 0x0051,
    SCANCODE_KP_0           = 0x0052,
    SCANCODE_KP_DOT         = 0x0053,

    SCANCODE_F11            = 0x0057,
    SCANCODE_F12            = 0x0058,

    // =====================
    // Extended keys (E0 prefix)
    // =====================
    SCANCODE_RIGHT_CTRL     = 0xE01D,
    SCANCODE_RIGHT_ALT      = 0xE038,
} keyboard_scancode_t;

static const char SCANCODE_TO_ASCII[128] = {
    [SCANCODE_ESC]          = 27,

    [SCANCODE_1]            = '1',
    [SCANCODE_2]            = '2',
    [SCANCODE_3]            = '3',
    [SCANCODE_4]            = '4',
    [SCANCODE_5]            = '5',
    [SCANCODE_6]            = '6',
    [SCANCODE_7]            = '7',
    [SCANCODE_8]            = '8',
    [SCANCODE_9]            = '9',
    [SCANCODE_0]            = '0',

    [SCANCODE_MINUS]        = '-',
    [SCANCODE_EQUAL]        = '=',
    [SCANCODE_BACKSPACE]    = '\b',
    [SCANCODE_TAB]          = '\t',

    [SCANCODE_Q]            = 'q',
    [SCANCODE_W]            = 'w',
    [SCANCODE_E]            = 'e',
    [SCANCODE_R]            = 'r',
    [SCANCODE_T]            = 't',
    [SCANCODE_Y]            = 'y',
    [SCANCODE_U]            = 'u',
    [SCANCODE_I]            = 'i',
    [SCANCODE_O]            = 'o',
    [SCANCODE_P]            = 'p',

    [SCANCODE_LBRACKET]     = '[',
    [SCANCODE_RBRACKET]     = ']',
    [SCANCODE_ENTER]        = '\n',

    [SCANCODE_A]            = 'a',
    [SCANCODE_S]            = 's',
    [SCANCODE_D]            = 'd',
    [SCANCODE_F]            = 'f',
    [SCANCODE_G]            = 'g',
    [SCANCODE_H]            = 'h',
    [SCANCODE_J]            = 'j',
    [SCANCODE_K]            = 'k',
    [SCANCODE_L]            = 'l',

    [SCANCODE_SEMICOLON]    = ';',
    [SCANCODE_APOSTROPHE]   = '\'',
    [SCANCODE_GRAVE]        = '`',

    [SCANCODE_BACKSLASH]    = '\\',

    [SCANCODE_Z]            = 'z',
    [SCANCODE_X]            = 'x',
    [SCANCODE_C]            = 'c',
    [SCANCODE_V]            = 'v',
    [SCANCODE_B]            = 'b',
    [SCANCODE_N]            = 'n',
    [SCANCODE_M]            = 'm',

    [SCANCODE_COMMA]        = ',',
    [SCANCODE_DOT]          = '.',
    [SCANCODE_SLASH]        = '/',

    [SCANCODE_SPACE]        = ' ',

    [SCANCODE_KP_0]         = '0',
    [SCANCODE_KP_1]         = '1',
    [SCANCODE_KP_2]         = '2',
    [SCANCODE_KP_3]         = '3',
    [SCANCODE_KP_4]         = '4',
    [SCANCODE_KP_5]         = '5',
    [SCANCODE_KP_6]         = '6',
    [SCANCODE_KP_7]         = '7',
    [SCANCODE_KP_8]         = '8',
    [SCANCODE_KP_9]         = '9',
    [SCANCODE_KP_DOT]       = '.',
};

static const char SCANCODE_TO_ASCII_SHIFT[128] = {
    [SCANCODE_1]            = '!',
    [SCANCODE_2]            = '@',
    [SCANCODE_3]            = '#',
    [SCANCODE_4]            = '$',
    [SCANCODE_5]            = '%',
    [SCANCODE_6]            = '^',
    [SCANCODE_7]            = '&',
    [SCANCODE_8]            = '*',
    [SCANCODE_9]            = '(',
    [SCANCODE_0]            = ')',

    [SCANCODE_MINUS]        = '_',
    [SCANCODE_EQUAL]        = '+',

    [SCANCODE_Q]            = 'Q',
    [SCANCODE_W]            = 'W',
    [SCANCODE_E]            = 'E',
    [SCANCODE_R]            = 'R',
    [SCANCODE_T]            = 'T',
    [SCANCODE_Y]            = 'Y',
    [SCANCODE_U]            = 'U',
    [SCANCODE_I]            = 'I',
    [SCANCODE_O]            = 'O',
    [SCANCODE_P]            = 'P',

    [SCANCODE_LBRACKET]     = '{',
    [SCANCODE_RBRACKET]     = '}',

    [SCANCODE_A]            = 'A',
    [SCANCODE_S]            = 'S',
    [SCANCODE_D]            = 'D',
    [SCANCODE_F]            = 'F',
    [SCANCODE_G]            = 'G',
    [SCANCODE_H]            = 'H',
    [SCANCODE_J]            = 'J',
    [SCANCODE_K]            = 'K',
    [SCANCODE_L]            = 'L',

    [SCANCODE_SEMICOLON]    = ':',
    [SCANCODE_APOSTROPHE]   = '"',
    [SCANCODE_GRAVE]        = '~',

    [SCANCODE_BACKSLASH]    = '|',

    [SCANCODE_Z]            = 'Z',
    [SCANCODE_X]            = 'X',
    [SCANCODE_C]            = 'C',
    [SCANCODE_V]            = 'V',
    [SCANCODE_B]            = 'B',
    [SCANCODE_N]            = 'N',
    [SCANCODE_M]            = 'M',

    [SCANCODE_COMMA]        = '<',
    [SCANCODE_DOT]          = '>',
    [SCANCODE_SLASH]        = '?',
};

typedef struct {
    uint8_t keycode;
    keyboard_scancode_t scancode;
    bool is_pressed;
    bool shift_held;
    bool ctrl_held;
    bool alt_held;
    bool caps_locked;
} keyboard_event_t;

void keyboard_initialize();

void keyboard_handler(const isr_frame_t *frame);

bool keyboard_has_event();

void keyboard_get_event(keyboard_event_t *event);

size_t keyboard_scancode_to_ascii(const keyboard_event_t *event, char *ascii);

#endif //KEYBOARD_H
