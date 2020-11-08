#pragma once

#include <std/types.h>

byte port_byte_in(uint16_t port);
void port_byte_out(uint16_t port, byte data);
uint16_t port_word_in(uint16_t port);
void port_word_out(uint16_t port, uint16_t data);