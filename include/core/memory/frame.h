#pragma once

#include <core/boot.h>
#include <std/io.h>

#define PAGE_SIZE       4096
// 256 mb
#define MAX_FRAMES      (256 * 1000000) / PAGE_SIZE

#define MULTIBOOT_MEMORY_AVAILABLE  1
#define MULTIBOOT_TAG_TYPE_MMAP             6
#define MULTIBOOT_TAG_TYPE_END              0

#define DEBUG(x, ...) printf(x "\n", __VA_ARGS__)
#define PANIC(x, ...) DEBUG(x, __VA_ARGS__)

opt_uint64_t frame_allocate();
void frame_deallocate(uint64_t frame_number);
uint64_t frame_containing_address(uint64_t addr);
uint64_t frame_starting_address(uint64_t frame_number);