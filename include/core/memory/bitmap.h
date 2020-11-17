#pragma once

#include <std/types.h>

typedef uint64_t bitmap_t;

enum { BITS_PER_WORD = sizeof(bitmap_t) * 8};

#define WORD_OFFSET(b)  ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b)   ((b) % BITS_PER_WORD)

bool bitmap_get(bitmap_t* b, uint64_t n);

void bitmap_set(bitmap_t* b, uint64_t n);

void bitmap_clear(bitmap_t* b, uint64_t n);