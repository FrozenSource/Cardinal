#pragma once

#include <std/types.h>
#include <drivers/driver.h>

// All the I/O ports are calculated relative to the data port. This is because
// all serial ports (COM1, COM2, COM3, COM4) have their ports in the same
// order, but they start at different values.

enum eSerialPort : uint16_t {
    COM1 = 0x3F8,
    COM2 = 0x2F8,
    COM3 = 0x3E8,
    COM4 = 0x2E8
};       

enum eSerialBaud : uint16_t {
    RATE_115200 = 1,
    RATE_57600 = 2,
    RATE_38400 = 3
};

class cSerialPort : public IDriver {
public:
    cSerialPort(eSerialPort ePort, eSerialBaud eRate);

    char Read();
    void Write(char c);
    void Write(cstring str);
protected:
    bool Init() { this->pbInitialized = true; return true; }
    bool Destroy() { this->pbInitialized = false; return true; }

    static uint32_t serial_received(uint32_t com);
    static uint32_t serial_is_transmit_fifo_empty(uint32_t com);

    eSerialPort pePort;
    eSerialBaud peRate;
};