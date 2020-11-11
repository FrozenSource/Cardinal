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

C_FUNCTION void kmain(uint64_t ulMagic, uint64_t ulMBIBegin) __asm__("kmain");
C_FUNCTION void kmain(uint64_t ulMagic, uint64_t ulMBIBegin)
{
    cStaticTerminalDriver::Get().Clear();
    printf("CardinalOS\n");
    printf("Kernel version: %s\n", VERSION_STR);
    printf("Build on %s at %s\n", __DATE__, __TIME__);

    cSystemInformationProvider& oSystemInf = cSystemInformationProvider::Get();
    if (!oSystemInf.Init(ulMagic, ulMBIBegin)) {
        printf("System halted!");
        return;
    }

    uint64_t ulTotalMemory = oSystemInf.GetTotalMemorySize(), ulUsedMemory = 0;
    for (uint8_t uiIndex = 0; uiIndex < oSystemInf.GetMemorySectionCount(); uiIndex++) {
        tMemorySection& oSection = oSystemInf.GetMemorySection(uiIndex);
        if (oSection.eType != eMemoryType::AVAILABLE) {
            ulUsedMemory += oSection.ulLength;
            continue;
        }
    }
    printf("Total Memory:   %s\n", formatBytes(ulTotalMemory, 2, eByteSize::MEGABYTES));
    printf("Used Memory:    %s\n", formatBytes(ulUsedMemory, 2, eByteSize::MEGABYTES));
    printf("Free Memory:    %s\n", formatBytes(ulTotalMemory - ulUsedMemory, 2, eByteSize::MEGABYTES));

    Setup_Interrupts();

    cStaticTimer::Get().Init();
    cStaticKeyBoardDriver::Get().Init();

    while (1) {
        __asm__("hlt");
    }
}
