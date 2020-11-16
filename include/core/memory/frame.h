#pragma once

#include <core/boot.h>
#include <std/io.h>

#define PAGE_SIZE       4096

#define MULTIBOOT_MEMORY_AVAILABLE  1
#define MULTIBOOT_TAG_TYPE_MMAP             6
#define MULTIBOOT_TAG_TYPE_END              0

#define DEBUG(x, ...) printf(x "\n", __VA_ARGS__)
#define PANIC(x, ...) DEBUG(x, __VA_ARGS__)

struct multiboot_info_t
{
  uint32_t size;
  uint32_t reserved;
  multiboot_tag_t tags[];
} PACKED;

void* find_multiboot_tag(multiboot_info_t* mbi, uint16_t type);
void mmap_init();
uint64_t mmap_allocate_frame();
void mmap_deallocate_frame(uint64_t frame_number);
uint64_t frame_containing_address(uint64_t addr);
uint64_t frame_starting_address(uint64_t frame_number);