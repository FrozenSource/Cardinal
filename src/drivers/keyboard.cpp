#include <drivers/keyboard.h>
#include <std/io.h>
#include <core/io/isr.h>
#include <core/io/ports.h>

#define SCANCODE_MAX 57
#define KEYBOARD_DATA_PORT 0x60

static void keyboard_callback(UNUSED uint64_t stack) {
    uint8_t scancode = port_byte_in(KEYBOARD_DATA_PORT);

    if (scancode > SCANCODE_MAX) {
        return;
    }

    printf("Received: %i\n", scancode);
}

bool cStaticKeyBoardDriver::Init() {
    if (IsInitialized()) return true;
    
    register_interrupt_handler(IRQ1, keyboard_callback);
    this->pbInitialized = true;
    return true;
}