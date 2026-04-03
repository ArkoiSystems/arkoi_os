#ifndef PIT_H
#define PIT_H

#include "arch/x86/idt.h"

void pit_initialize();

void pit_handler(const isr_frame_t* frame);

#endif // PIT_H
