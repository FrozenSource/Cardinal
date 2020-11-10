#pragma once

#include <core/io/idt.h>

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34	
#define IRQ3 35
#define IRQ4 36

typedef void (*isr_t) (uint64_t stack);
void register_interrupt_handler(uint64_t id, isr_t handler);

void Setup_Interrupts();
