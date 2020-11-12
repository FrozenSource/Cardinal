#include <std/io.h>

#include <std/string.h>
#include <std/arg.h>
#include <drivers/screen.h>
#include <std/convert.h>

void printf(cstring sFormat, ...)
{
    va_list aArgs;
    va_start(aArgs, sFormat);

    int64_t ilValue;
    uint64_t ulValue;
    double dValue;
    for (memsize_t uiIndex = 0; uiIndex < strlen(sFormat); uiIndex++) {
        char cCharacter = sFormat[uiIndex];

        if (cCharacter == '%') {
            switch (sFormat[uiIndex + 1]) {
                case 'c':
                    putchar(va_arg(aArgs, int64_t));
                    break;

                case 'i':
                    ilValue = va_arg(aArgs, int64_t);
                    puts(itoa(ilValue, 10));
                    break;
                
                case 'u':
                    ulValue = va_arg(aArgs, uint64_t);
                    puts(itoa(ulValue, 10));
                    break;

                case 'd':
                    dValue = va_arg(aArgs, double);
                    puts(dtoa(dValue, 2));
                    break;

                case 'x':
                    ilValue = va_arg(aArgs, uint64_t);
                    puts(itoa(ilValue, 16));
                    break;

                case 's':
                    puts(va_arg(aArgs, cstring));
                    break;

                case 'b':
                    ilValue = va_arg(aArgs, int);
                    
                    if (ilValue == true) {
                        puts("true");
                    } else {
                        puts("false");
                    }
                    break;
            }

            uiIndex++;
        } else {
            putchar(cCharacter);
        }
    }

    va_end(aArgs);
}

void putchar(char cCharacter)
{
    cStaticTerminalDriver::Get().Print(cCharacter);
}

void puts(cstring sStr)
{
    cStaticTerminalDriver::Get().Print(sStr);
}
