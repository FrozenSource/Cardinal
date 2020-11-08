#include <std/io.h>

#include <std/string.h>
#include <std/arg.h>
#include <drivers/screen.h>
#include <std/convert.h>

void printf(const cstring format, ...)
{
    va_list arg;
    va_start(arg, format);

    int i_val;
    char s_val[20];
    for (memsize_t i = 0; i < strlen(format); i++) {
        char c = format[i];

        if (c == '%') {
            switch (format[i + 1]) {
                case 'c':
                    putchar(va_arg(arg, int));
                    break;

                case 'd':
                    i_val = va_arg(arg, int);
                    if (i_val < 0) {
                        i_val = -i_val;
                        putchar('-');
                    }
                    itoa(i_val, s_val);
                    puts(s_val);
                    break;

                case 'x':
                    i_val = va_arg(arg, unsigned int);
                    puts(convert(i_val, 16));
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

char* convert(unsigned int num, int base)
{
    char representation[] = "0123456789ABCDEF";
    char buffer[50];
    char *ret;

    ret = &buffer[49];
    *ret = '\0';

    do {
        *--ret = representation[num % base];
        num /= base;
    } while(num != 0);

    return ret;
}

void putchar(char c)
{
    screen_printc(c);
}

void puts(char *s)
{
    screen_print(s);
}
