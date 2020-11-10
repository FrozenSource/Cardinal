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
    cStaticTerminalDriver::Get().Clear();
    printf("CardinalOS\n");
    printf("Kernel version: %s\n", VERSION_STR);
    printf("Build on %s at %s\n", __DATE__, __TIME__);

    isr_init();
    irq_init();

    cStaticTimer::Get().Init();
    cStaticKeyBoardDriver::Get().Init();

    while (1) {
        __asm__("hlt");
    }
}
