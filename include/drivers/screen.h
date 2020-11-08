#pragma once

#include <core/memory/locations.h>
#include <std/types.h>
#include <core/memory/memory.h>

static const uint8_t COLOR_BLACK         = 0;
static const uint8_t COLOR_BLUE          = 1;
static const uint8_t COLOR_GREEN         = 2;
static const uint8_t COLOR_CYAN          = 3;
static const uint8_t COLOR_RED           = 4;
static const uint8_t COLOR_MAGENTA       = 5;
static const uint8_t COLOR_BROWN         = 6;
static const uint8_t COLOR_LIGHT_GREY    = 7;
static const uint8_t COLOR_DARK_GREY     = 8;
static const uint8_t COLOR_LIGHT_BLUE    = 9;
static const uint8_t COLOR_LIGHT_GREEN   = 10;
static const uint8_t COLOR_LIGHT_CYAN    = 11;
static const uint8_t COLOR_LIGHT_RED     = 12;
static const uint8_t COLOR_LIGHT_MAGENTA = 13;
static const uint8_t COLOR_LIGHT_BROWN   = 14;
static const uint8_t COLOR_WHITE         = 15;

void screen_init();
void screen_clear();
void screen_line(const cstring str);
void screen_print(const cstring str);
void screen_print_line(const cstring str);
uint8_t color_scheme(uint8_t fg, uint8_t bg);
void move_cursor(uint16_t pos);
