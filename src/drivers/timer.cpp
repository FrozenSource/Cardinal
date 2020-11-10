#include <drivers/timer.h>
#include <core/io/isr.h>
#include <std/convert.h>
#include <core/io/ports.h>

#define C_GLOBAL_TICK 50
uint32_t guiTicker = 0;

static void timer_callback(UNUSED uint64_t stack) {
    guiTicker++;
}

uint32_t cStaticTimer::GetTick() {
    return guiTicker;
}

bool cStaticTimer::Init() {
    if (IsInitialized()) return true;

    register_interrupt_handler(IRQ0, timer_callback);
    int32_t divisor = 1193180 / C_GLOBAL_TICK;
    port_byte_out(0x43, 0x36);
    port_byte_out(0x40, divisor & 0xff);
    port_byte_out(0x40, divisor >> 8);

    this->pbInitialized = true;
    return true;
}

bool cStaticTimer::Destroy() {
    this->pbInitialized = false;
    return true;
}