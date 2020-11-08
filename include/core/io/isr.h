#pragma once

#include <core/io/idt.h>

// These functions are declared in interrupts.asm file
extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();

void isr_init();
void irq_init();
void isr_handler(uint64_t id, uint64_t stack) __asm__("isr_handler");


// IRQ

extern "C" void irq0();
extern "C" void irq1();
extern "C" void irq2();
extern "C" void irq3();
extern "C" void irq4();

typedef void (*isr_t) (uint64_t stack);

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34	
#define IRQ3 35
#define IRQ4 36

#define PIC1        0x20 // Master PIC
#define PIC2        0xA0 // Slave PIC
#define PIC1_DATA   (PIC1 + 1)
#define PIC2_DATA   (PIC2 + 1)
#define PIC_EOI     0x20 // end of interrupt
#define IRQ_BASE    0x20

void irq_handler(uint64_t id, uint64_t stack) __asm__("irq_handler");
void register_interrupt_handler(uint64_t id, isr_t handler);
