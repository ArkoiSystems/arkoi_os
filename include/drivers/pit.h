#ifndef PIT_H
#define PIT_H

#include "arch/x86/idt/idt.h"

void pit_initialize();

void pit_handler(const isr_frame_t* frame);

void pit_wait(uint32_t milliseconds);

#endif // PIT_H
