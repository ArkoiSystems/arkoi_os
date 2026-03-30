#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "arch/x86/idt.h"

void keyboard_initialize();

void keyboard_handler(const isr_frame_t *frame);

#endif //KEYBOARD_H
