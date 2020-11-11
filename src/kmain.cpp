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
#include <core/boot.h>

C_FUNCTION void kmain(uint64_t ulMagic, uint64_t ulAddr) __asm__("kmain");
C_FUNCTION void kmain(uint64_t ulMagic, uint64_t ulAddr)
{
    cStaticTerminalDriver::Get().Clear();
    printf("CardinalOS\n");
    printf("Kernel version: %s\n", VERSION_STR);
    printf("Build on %s at %s\n", __DATE__, __TIME__);

    cStaticBootProvider& oBootProvider = cStaticBootProvider::Get();
    oBootProvider.Init(ulMagic, ulAddr);
    if (!oBootProvider.IsBootOk()) {
        printf("System halted!");
        return;
    }
    oBootProvider.GetBootInfo();

    Setup_Interrupts();

    cStaticTimer::Get().Init();
    cStaticKeyBoardDriver::Get().Init();

    while (1) {
        __asm__("hlt");
    }
}
