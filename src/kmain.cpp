#include <std/types.h>
#include <drivers/screen.h>
#include <Version.h>
#include <core/io/isr.h>
#include <core/memory/memory.h>
#include <drivers/keyboard.h>
#include <drivers/timer.h>
#include <drivers/serial.h>
#include <std/io.h>
#include <std/convert.h>
#include <std/string.h>

C_FUNCTION void kmain(unsigned long magic, unsigned long addr) __asm__("kmain");
C_FUNCTION void kmain(unsigned long magic, unsigned long addr)
{
    cStaticTerminalDriver::Get().Clear();
    printf("CardinalOS\n");
    printf("Kernel version: %s\n", VERSION_STR);
    printf("Build on %s at %s\n", __DATE__, __TIME__);

    printf("Magic: 0x%x\n", magic);
    printf("Addr: 0x%x\n", addr);

    Setup_Interrupts();

    float f = 333.715f;
    double d = -9.4;
    printf("i: %i\n", -1);
    printf("f: %d\n", f);
    printf("d: %d\n", d);

    cStaticTimer::Get().Init();
    cStaticKeyBoardDriver::Get().Init();

    while (1) {
        __asm__("hlt");
    }
}
