#include <core/memory/frame.h>
#include <std/io.h>

#define MULTIBOOT_MEMORY_AVAILABLE  1
#define MULTIBOOT_TAG_TYPE_MMAP             6
#define MULTIBOOT_TAG_TYPE_END              0

#define PANIC(x, ...) printf(x, __VA_ARGS__)
#define DEBUG(x, ...) printf(x, __VA_ARGS__)

struct multiboot_tag_t {
    uint32_t type;
    uint32_t size;
} PACKED;

struct multiboot_info_t
{
  uint32_t size;
  uint32_t reserved;
  multiboot_tag_t tags[];
} PACKED;

struct multiboot_mmap_entry_t {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t zero;
} PACKED;

struct multiboot_tag_mmap_t {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    multiboot_mmap_entry_t entries[];
} PACKED;

multiboot_tag_mmap_t* memory_area;
uint64_t kernel_start;
uint64_t kernel_end;
uint64_t multiboot_start;
uint64_t multiboot_end;
uint64_t next_free_frame;

void* find_multiboot_tag(multiboot_info_t* mbi, uint16_t type) {
  // `*tag` points to the first tag of the multiboot_info_t struct
  for (multiboot_tag_t* tag = mbi->tags; tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (multiboot_tag_t*)((uint8_t*)tag + ((tag->size + 7) & ~7))) {
    if (tag->type == type) {
      return tag;
    }
  }

  return 0;
}

void mmap_init() {
    cSystemInformationProvider& oSystemInf = cSystemInformationProvider::Get();
    kernel_start = (uint64_t) oSystemInf.GetKernelBegin();
    kernel_end = (uint64_t) oSystemInf.GetKernelEnd();
    multiboot_start = (uint64_t) oSystemInf.GetBootSectionBegin();
    multiboot_end = (uint64_t) oSystemInf.GetBootSectionEnd();
    next_free_frame = 1;

    multiboot_info_t* mbi = (multiboot_info_t*) multiboot_start;
    multiboot_tag_mmap_t* mmap = (multiboot_tag_mmap_t*) find_multiboot_tag(mbi, MULTIBOOT_TAG_TYPE_MMAP);
    memory_area = mmap;
}

uint64_t mmap_read(uint64_t request, uint8_t mode) {
    uint64_t cur_num = 0;

    for (
        multiboot_mmap_entry_t* entry = memory_area->entries;
        (uint8_t*) entry < (uint8_t*) memory_area + memory_area->size;
        entry = (multiboot_mmap_entry_t*)((uint64_t) entry + memory_area->entry_size)
    ) {
        if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
            continue;
        }

        uint64_t entry_end = entry->addr + entry->len;

        for (uint64_t i = entry->addr; i + PAGE_SIZE < entry_end; i += PAGE_SIZE) {
            if ((i >= multiboot_start && i <= multiboot_end) || (i >= kernel_start && i <= kernel_end)) {
                continue;
            }

            if (mode == MMAP_GET_NUM && request >= i && request <= i + PAGE_SIZE) {
                return cur_num + 1;
            }

            if (mode == MMAP_GET_ADDR && cur_num == request && i != 0) {
                return i;
            }

            cur_num++;
        }
    }

    return 0;
}

uint64_t mmap_allocate_frame() {
    // Get the address for the next free frame
    uint64_t addr = mmap_read(next_free_frame, MMAP_GET_ADDR);

    if (addr == 0) {
        PANIC("failed to allocate a new frame, addr=%p", addr);
    }

    DEBUG("allocated new frame with addr=%p", addr);

    return next_free_frame++;
}

void mmap_deallocate_frame(uint64_t addr) {
    uint64_t frame = frame_starting_address(addr);

    for (int i = 0; i < PAGE_SIZE; i++) {
        ((uint64_t*)frame)[i] = 0;
    }
}

uint64_t frame_containing_address(uint64_t addr) {
    return addr / PAGE_SIZE;
}

uint64_t frame_starting_address(uint64_t addr) {
    return addr * PAGE_SIZE;
}