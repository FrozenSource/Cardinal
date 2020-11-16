#include <core/memory/frame.h>
#include <std/io.h>

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

uint64_t mmap_read(uint64_t request) {
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

        for (uint64_t addr = entry->addr; addr + PAGE_SIZE < entry_end; addr += PAGE_SIZE) {
            if ((addr >= multiboot_start && addr <= multiboot_end) || (addr >= kernel_start &&
                    addr <= kernel_end)) {
                continue;
            }

            if (cur_num == request && addr != 0) {
                return addr;
            }

            cur_num++;
        }
    }

    return 0;
}

uint64_t mmap_allocate_frame() {
    // Get the address for the next free frame
    uint64_t addr = mmap_read(next_free_frame++);

    if (addr == 0) {
        PANIC("failed to allocate a new frame, addr=%p", addr);
    }

    DEBUG("allocated new frame with addr=%p", addr);

    return addr;
}

void mmap_deallocate_frame(uint64_t frame_number) {
    uint64_t addr = frame_starting_address(frame_number);
    memset((byte*) addr, 0, sizeof(uint64_t) * PAGE_SIZE);
}

uint64_t frame_containing_address(uint64_t addr) {
    return addr / PAGE_SIZE;
}

uint64_t frame_starting_address(uint64_t frame_number) {
    return frame_number * PAGE_SIZE;
}