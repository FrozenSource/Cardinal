#include <drivers/serial.h>
#include <core/io/ports.h>
#include <std/string.h>

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

// Tells the serial port to expect first the highest 8 bits on the data port,
// then the lowest 8 bits will follow.
#define SERIAL_LINE_ENABLE_DLAB         0x80

cSerialPort::cSerialPort(eSerialPort ePort, eSerialBaud eRate) : pePort(ePort), peRate(eRate) {
    port_byte_out(SERIAL_LINE_COMMAND_PORT(this->pePort), SERIAL_LINE_ENABLE_DLAB);
    port_byte_out(SERIAL_DATA_PORT(this->pePort),         (this->peRate >> 8) & 0x00FF);
    port_byte_out(SERIAL_DATA_PORT(this->pePort),         this->peRate & 0x00FF);

    // cf. https://littleosbook.github.io/#configuring-the-serial-port
    //
    // Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
    // Content: | d | b | prty  | s | dl  |
    // Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
    port_byte_out(SERIAL_LINE_COMMAND_PORT(this->pePort), 0x03);

    // Enable FIFO, clear them, with 14b threshold
    port_byte_out(SERIAL_FIFO_COMMAND_PORT(this->pePort), 0xC7);

    // IRQs enabled, RTS/DSR set
    port_byte_out(SERIAL_MODEM_COMMAND_PORT(this->pePort), 0x0B);
}

char cSerialPort::Read() {
    while (serial_received(this->pePort) == 0) ;

    return port_byte_in(this->pePort);
}

void cSerialPort::Write(char c) {
    while (serial_is_transmit_fifo_empty(this->pePort) == 0) ;

    port_byte_out(this->pePort, c);
}

void cSerialPort::Write(cstring str) {
    for (memsize_t uiIndex = 0; uiIndex < strlen(str); uiIndex++) {
        Write(str[uiIndex]);
    }
}

uint32_t cSerialPort::serial_is_transmit_fifo_empty(uint32_t com)
{
    /* 0x20 = 0010 0000 */
    return port_byte_in(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

uint32_t cSerialPort::serial_received(uint32_t com)
{
    return port_byte_in(SERIAL_LINE_STATUS_PORT(com)) & 1;
}
