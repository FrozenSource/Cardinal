#include <std/types.h>
#include <drivers/screen.h>
#include <Version.h>
#include <core/io/isr.h>
#include <core/memory/memory.h>
#include <drivers/keyboard.h>
#include <drivers/timer.h>
#include <drivers/serial.h>
#include <std/io.h>

extern "C" void kmain()
{
    screen_init();
    screen_clear();
    screen_line(nullptr);
    printf("CardinalOS\n");
    printf("Kernel version: %s\n", VERSION_STR);
    printf("Build on %s at %s\n", __DATE__, __TIME__);
    screen_line(nullptr);

    isr_init();
    screen_print_line("ISR initialized.");
    irq_init();
    screen_print_line("ISQ initialized.");

    timer_init(50);
    cStaticKeyBoardDriver& oKeyBoard = cStaticKeyBoardDriver::Get();
    oKeyBoard.Init();

    cSerialPort oPort(eSerialPort::COM1, eSerialBaud::RATE_115200);
    oPort.Write("hallo");

    while (1) {
        __asm__("hlt");
    }
}
