#include <drivers/timer.h>
#include <core/io/isr.h>
#include <std/convert.h>
#include <core/io/ports.h>

uint32_t tick = 0;

static void timer_callback(UNUSED uint64_t stack) {
    tick++;
}

void timer_init(int32_t freq) {
    register_interrupt_handler(IRQ0, timer_callback);

    int32_t divisor = 1193180 / freq;

    port_byte_out(0x43, 0x36);
    port_byte_out(0x40, divisor & 0xff);
    port_byte_out(0x40, divisor >> 8);
}