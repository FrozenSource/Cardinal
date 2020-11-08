#include <drivers/serial.h>
#include <core/io/ports.h>

int serial_received(uint32_t com);
int serial_is_transmit_fifo_empty(uint32_t com);

void serial_init(uint16_t com, uint16_t divisor)
{
    port_byte_out(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
    port_byte_out(SERIAL_DATA_PORT(com),         (divisor >> 8) & 0x00FF);
    port_byte_out(SERIAL_DATA_PORT(com),         divisor & 0x00FF);

    // cf. https://littleosbook.github.io/#configuring-the-serial-port
    //
    // Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
    // Content: | d | b | prty  | s | dl  |
    // Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
    port_byte_out(SERIAL_LINE_COMMAND_PORT(com), 0x03);

    // Enable FIFO, clear them, with 14b threshold
    port_byte_out(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);

    // IRQs enabled, RTS/DSR set
    port_byte_out(SERIAL_MODEM_COMMAND_PORT(com), 0x0B);
}

int serial_is_transmit_fifo_empty(uint32_t com)
{
    /* 0x20 = 0010 0000 */
    return port_byte_in(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

int serial_received(uint32_t com)
{
    return port_byte_in(SERIAL_LINE_STATUS_PORT(com)) & 1;
}

char serial_read(uint32_t com)
{
    while (serial_received(com) == 0) ;

    return port_byte_in(com);
}

void serial_write(uint32_t com, char c) {
    while (serial_is_transmit_fifo_empty(com) == 0) ;

    port_byte_out(com, c);
}