#include <std/io.h>

#include <std/string.h>
#include <std/arg.h>
#include <drivers/screen.h>
#include <std/convert.h>

void printf(cstring format, ...)
{
    va_list arg;
    va_start(arg, format);

    int i_val;
    uint32_t u_val;
    double d_val;
    for (memsize_t i = 0; i < strlen(format); i++) {
        char c = format[i];

        if (c == '%') {
            switch (format[i + 1]) {
                case 'c':
                    putchar(va_arg(arg, char));
                    break;

                case 'i':
                    i_val = va_arg(arg, int32_t);
                    puts(itoa(i_val, 10));
                    break;
                
                case 'u':
                    u_val = va_arg(arg, uint32_t);
                    puts(itoa(u_val, 10));
                    break;

                case 'd':
                    d_val = va_arg(arg, double);
                    puts(dtoa(d_val, 2));
                    break;

                case 'x':
                    i_val = va_arg(arg, uint32_t);
                    puts(itoa(i_val, 16));
                    break;

                case 's':
                    puts(va_arg(arg, char *));
                    break;
            }

            i++;
        } else {
            putchar(c);
        }
    }

    va_end(arg);
}

void putchar(char c)
{
    cStaticTerminalDriver::Get().Print(c);
}

void puts(char *s)
{
    cStaticTerminalDriver::Get().Print((cstring)s);
}
