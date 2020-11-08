#include <drivers/keyboard.h>
#include <std/types.h>
#include <core/io/isr.h>
#include <drivers/screen.h>
#include <std/convert.h>
#include <core/io/ports.h>

#define SCANCODE_MAX 57

static void keyboard_callback(UNUSED uint64_t stack)
{
    uint8_t scancode = port_byte_in(KEYBOARD_DATA_PORT);

    if (scancode > SCANCODE_MAX) {
        return;
    }

    char s[3];
    itoa(scancode, s);

    screen_print("Received: ");
    screen_print(s);
    screen_print("\n");
}

void keyboard_init()
{
    register_interrupt_handler(IRQ1, keyboard_callback);
}