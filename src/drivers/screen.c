#include <drivers/screen.h>
#include <std/string.h>

uint8_t screen_scheme;
byte* framebuffer;
int screen_col;
int screen_row;

#define VGA_WIDTH       80
#define VGA_HEIGHT      24

void print_char_at(char c, uint8_t scheme, uint32_t x, uint32_t y);
void print_char(char c);

uint8_t color_scheme(uint8_t fg, uint8_t bg) {
    return fg | bg << 4;
}

void screen_init()
{
    framebuffer = (byte*) C_MEMORY_VGA_VIDEO_ADDRESS;
    screen_scheme = color_scheme(COLOR_LIGHT_GREY, COLOR_BLACK);
    screen_col = 0;
    screen_row = 0;
}

void screen_line(const cstring str) {
    memsize_t uiStrSize = str > (cstring) nullptr ? CStrLength(str) : 0;
    if (screen_col != 0) print_char('\n');
    for (memsize_t uiIndex = screen_col; uiIndex < VGA_WIDTH; uiIndex++) {
        if (uiIndex < uiStrSize) {
            print_char(str[uiIndex]);
        } else {
            print_char('-');
        }
    }
}

void screen_print(const cstring str)
{
    for (memsize_t i = 0; i < CStrLength(str); i++) {
        print_char(str[i]);
    }
}

void screen_print_line(const cstring str)
{
    screen_print(str);
    screen_row++;
    screen_col = 0;
}

void screen_clear() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            print_char_at(' ', screen_scheme, x, y);
        }
    }
}

void print_char_at(char c, uint8_t scheme, uint32_t x, uint32_t y) {
    const int offset = 2 * (y * VGA_WIDTH + x);

    framebuffer[offset] = c;
    framebuffer[offset + 1] = scheme;

    // scrolling
    if (offset > VGA_HEIGHT * VGA_WIDTH * 2) {
        for (memsize_t i = 1; i < VGA_HEIGHT; i++) {
            memcpy(
                    (void *) (C_MEMORY_VGA_VIDEO_ADDRESS + (2 * i * VGA_WIDTH)),
                    (const void *) (C_MEMORY_VGA_VIDEO_ADDRESS + (2 * (i - 1) * VGA_WIDTH)),
                    2 * VGA_WIDTH
                  );
        }

        char* last_line = (char *)(2 * (VGA_HEIGHT) * VGA_WIDTH + C_MEMORY_VGA_VIDEO_ADDRESS);
        for (int i = 0; i < VGA_WIDTH * 2; i++) {
            last_line[i] = 0;
        }

        screen_row--;
    }
}

void print_char(char c) {
    if (c == '\n') {
        screen_col = 0;
        if (++screen_row >= VGA_HEIGHT) screen_row = 0;
        return;
    } else if (c == '\t') {
        screen_col = screen_col + 4 - (screen_col % 4);
        return;
    } else if (c == '\r') {
        screen_col = 0;
        return;
    }

    print_char_at(c, screen_scheme, screen_col, screen_row);

    if (++screen_col >= VGA_WIDTH)
    {
        screen_col = 0;
        if (++screen_row >= VGA_HEIGHT)
        {
            screen_row = 0;
        }
    }
}