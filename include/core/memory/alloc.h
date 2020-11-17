#pragma once

#include <std/types.h>

#define HEAP_START  0x40000000
#define HEAP_SIZE   200 * 1024 // 200 KiB

/**
 * Initializes the heap memory allocator.
 */
void alloc_init();
int liballoc_lock();
int liballoc_unlock();
void* liballoc_alloc(uint32_t number_of_pages);
int liballoc_free(void* ptr, uint32_t number_of_pages);