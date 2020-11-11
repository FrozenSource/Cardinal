#pragma once

#include <std/types.h>

void set_idt_gate(uint16_t n, uint64_t handler);
void set_idt();