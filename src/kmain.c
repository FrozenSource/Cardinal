#include <std/types.h>
#include <drivers/screen.h>
#include <Version.h>
#include <core/io/isr.h>

void kmain()
{
    screen_init();
    screen_clear();
    screen_line(nullptr);
    screen_print_line("CardinalOS");
    screen_print("Kernel version: ");
    screen_print_line(VERSION_STR);
    screen_print("Build: ");
    screen_print(__DATE__);
    screen_print(" ");
    screen_print_line(__TIME__);
    screen_line(nullptr);

    isr_init();
    __asm__("int $3");
    __asm__("int $2");
}
