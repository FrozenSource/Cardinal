#include <core/memory/frame.h>
#include <std/io.h>
#include <core/memory/bitmap.h>

#define FRAME_BITMAP_SIZE MAX_FRAMES / BITS_PER_WORD

uint64_t kernel_start;
uint64_t kernel_end;
uint64_t multiboot_start;
uint64_t multiboot_end;
bitmap_t allocated_frames[FRAME_BITMAP_SIZE] { 0 };

opt_uint64_t mmap_read(uint64_t request) {
  uint64_t cur_num = 0;
  
  cSystemInformationProvider& oSystemInf = cSystemInformationProvider::Get();

  uint8_t uiIndex = 0;
  while (uiIndex < oSystemInf.GetMemorySectionCount()) {
    tMemorySection& oSection = oSystemInf.GetMemorySection(uiIndex);
    if (oSection.eType != eMemoryType::AVAILABLE) continue;

    uint64_t entry_end = oSection.pAddress + oSection.ulLength;

    for (uint64_t addr = oSection.pAddress; addr + PAGE_SIZE < entry_end;
         addr += PAGE_SIZE) {
      if ((addr >= (uint64_t) oSystemInf.GetBootSectionBegin() && addr <= (uint64_t) oSystemInf.GetBootSectionEnd()) ||
          (addr >= (uint64_t) oSystemInf.GetKernelBegin() && addr <= (uint64_t) oSystemInf.GetKernelEnd())) {
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