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
#include <core/memory/frame.h>
#include <core/memory/paging.h>
#include <core/boot.h>
#include <core/memory/alloc.h>
#include <std/memory.h>

char* strcpy(char *strDest, const char *strSrc) {
    char *temp = strDest;
    while((*strDest++=*strSrc++) != '\0');
    return temp;
}

int overflow() {
    char c[12];
    strcpy(c, "123456789012345678901234567890");
    return 1;
}

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

    uint64_t ulTotalMemory = oSystemInf.GetTotalMemorySize(), ulFreeMemory = 0, ulKernelEnd = (uint64_t) oSystemInf.GetKernelEnd();
    uint64_t ulFreeBegin = ulKernelEnd + 1024;
    for (uint8_t uiIndex = 0; uiIndex < oSystemInf.GetMemorySectionCount(); uiIndex++) {
        tMemorySection& oSection = oSystemInf.GetMemorySection(uiIndex);
        if (oSection.eType == eMemoryType::AVAILABLE) {
            if (oSection.pAddress < ulFreeBegin) {
                oSection.ulLength -= ulFreeBegin - oSection.pAddress;
                oSection.pAddress = ulFreeBegin;
            }

            ulFreeMemory += oSection.ulLength;
        }
    }

    printf("Memory Kernel Begin at: %s\n", formatBytes((uint64_t) oSystemInf.GetKernelBegin(), 2, eByteSize::KILOBYTES));
    printf("Memory Kernel End at: %s\n", formatBytes((uint64_t) oSystemInf.GetKernelEnd(), 2, eByteSize::KILOBYTES));
    printf("Memory Start Free at: %s\n", formatBytes(ulFreeBegin, 2, eByteSize::KILOBYTES));

    printf("Total Memory:   %s\n", formatBytes(ulTotalMemory, 2, eByteSize::MEGABYTES));
    printf("Used Memory:    %s\n", formatBytes(ulTotalMemory - ulFreeMemory, 2, eByteSize::MEGABYTES));
    printf("Free Memory:    %s\n", formatBytes(ulFreeMemory, 2, eByteSize::MEGABYTES));

    Setup_Interrupts();

    cStaticTimer::Get().Init();
    cStaticKeyBoardDriver::Get().Init();
    paging_init();
    alloc_init();
    
    void* pAddr1 = malloc(1024 * 1024);
    printf("p1: %p\n", pAddr1);
    free(pAddr1);

    overflow();

    while (1) {
        __asm__("hlt");
    }
}
