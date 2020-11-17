#include <core/memory/frame.h>
#include <std/io.h>
#include <core/memory/bitmap.h>

#define FRAME_BITMAP_SIZE MAX_FRAMES / BITS_PER_WORD

multiboot_tag_mmap_t* memory_area;
uint64_t kernel_start;
uint64_t kernel_end;
uint64_t multiboot_start;
uint64_t multiboot_end;
bitmap_t allocated_frames[FRAME_BITMAP_SIZE];

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

void frame_init() {
    cSystemInformationProvider& oSystemInf = cSystemInformationProvider::Get();
    kernel_start = (uint64_t) oSystemInf.GetKernelBegin();
    kernel_end = (uint64_t) oSystemInf.GetKernelEnd();
    multiboot_start = (uint64_t) oSystemInf.GetBootSectionBegin();
    multiboot_end = (uint64_t) oSystemInf.GetBootSectionEnd();
    memset(allocated_frames, 0, FRAME_BITMAP_SIZE);

    multiboot_info_t* mbi = (multiboot_info_t*) multiboot_start;
    multiboot_tag_mmap_t* mmap = (multiboot_tag_mmap_t*) find_multiboot_tag(mbi, MULTIBOOT_TAG_TYPE_MMAP);
    memory_area = mmap;
}

opt_uint64_t mmap_read(uint64_t request) {
  uint64_t cur_num = 0;

  for (multiboot_mmap_entry_t* entry = memory_area->entries;
       (uint8_t*)entry < (uint8_t*)memory_area + memory_area->size;
       entry =
         (multiboot_mmap_entry_t*)((uint64_t)entry + memory_area->entry_size)) {
    if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
      continue;
    }

    uint64_t entry_end = entry->addr + entry->len;

    for (uint64_t addr = entry->addr; addr + PAGE_SIZE < entry_end;
         addr += PAGE_SIZE) {
      if ((addr >= multiboot_start && addr <= multiboot_end) ||
          (addr >= kernel_start && addr <= kernel_end)) {
        continue;
      }

      if (cur_num == request) {
        return (opt_uint64_t){ .has_value = true, .value = addr };
      }

      cur_num++;
    }
  }

  return (opt_uint64_t){ .has_value = false, .value = 0 };
}

opt_uint64_t frame_allocate() {
    uint64_t frame_number  = 0;

    for (uint64_t i = 0; i < MAX_FRAMES; i++) {
        if (!bitmap_get(allocated_frames, i)) {
            frame_number  = i;
            break;
        }
    }

    opt_uint64_t frame = mmap_read(frame_number);
    if (frame.has_value) {
        DEBUG("allocated frame=%u addr=%p", frame_number, frame.value);
        bitmap_set(allocated_frames, frame_number);
    }
    
    return frame;
}

void frame_deallocate(uint64_t frame_number) {    
    bitmap_clear(allocated_frames, frame_number);
}

uint64_t frame_containing_address(uint64_t addr) {
    return addr / PAGE_SIZE;
}

uint64_t frame_starting_address(uint64_t frame_number) {
    return frame_number * PAGE_SIZE;
}