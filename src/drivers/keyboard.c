#include <drivers/keyboard.h>
#include <std/types.h>
#include <std/io.h>
#include <core/io/isr.h>
#include <std/convert.h>
#include <core/io/ports.h>

#define SCANCODE_MAX 57

static void keyboard_callback(UNUSED uint64_t stack)
{
    uint8_t scancode = port_byte_in(KEYBOARD_DATA_PORT);

    if (scancode > SCANCODE_MAX) {
        return;
    }

    printf("Received: %d\n", scancode);
}

void keyboard_init()
{
    register_interrupt_handler(IRQ1, keyboard_callback);
}