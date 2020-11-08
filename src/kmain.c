#include <stdint.h>
#include <drivers/screen.h>

void kmain(void)
{
    uint16_t* vga = (uint16_t*) VIDEO_ADDRESS;
    uint8_t scheme = new_scheme(COLOR_RED, COLOR_BLACK);

    vga[0] = vga_print('H', scheme);
    vga[1] = vga_print('a', scheme);
    vga[2] = vga_print('l', scheme);
    vga[3] = vga_print('l', scheme);
    vga[4] = vga_print('o', scheme);
    vga[5] = vga_print(' ', scheme);
    vga[6] = vga_print('b', scheme);
    vga[7] = vga_print('r', scheme);
    vga[8] = vga_print('a', scheme);
    vga[9] = vga_print('m', scheme);
}
