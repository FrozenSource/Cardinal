#include <core/io/idt.h>

#define IDT_ENTRIES 256

struct opts_t {
    uint8_t stack_OK  : 3;
    uint8_t ZEROS     : 5;
    uint8_t gate_type : 1;
    uint8_t ONES      : 3;
    uint8_t ZERO      : 1;
    uint8_t DPL       : 2;
    uint8_t present   : 1;
} PACKED;

struct idt_gate_t {
    uint16_t ptr_low;
    uint16_t selector;
    opts_t opts;
    uint16_t ptr_mid;
    uint32_t ptr_high;

    uint8_t  _1_reserved : 8;
    uint8_t  _type       : 5;
    uint32_t _2_reserved : 19;
} PACKED;

struct idt_register_t {
    uint16_t length;
    uint64_t base;
} PACKED;

idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

void set_idt_gate(uint16_t n, uint64_t handler)
{
    idt[n].selector = 0x08;
    idt[n].ptr_low  = (uint16_t) handler;
    idt[n].ptr_mid  = (uint16_t) (handler >> 16);
    idt[n].ptr_high = (uint32_t) (handler >> 32);

    idt[n].opts.stack_OK  = 0; // do not switch stack
    idt[n].opts.present   = 1; // are we valid
    idt[n].opts.DPL       = 3; // priv to call int handler
    idt[n].opts.gate_type = 0x01; // 1 = interrupt, 2 = trap
    idt[n].opts.ONES      = 0x07;
    idt[n].opts.ZERO      = 0;
    idt[n].opts.ZEROS     = 0;

    idt[n]._1_reserved = 0;
    idt[n]._2_reserved = 0;
    idt[n]._type       = 0;
}

void set_idt()
{
    idt_reg.base = (uint64_t) &idt;
    idt_reg.length = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    __asm__("lidt %0" : : "m"(idt_reg));
}