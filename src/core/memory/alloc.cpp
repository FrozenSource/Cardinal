#include <core/memory/alloc.h>
#include <core/memory/paging.h>
#include <core/memory/bitmap.h>

#define ALLOC_BITMAP_SIZE (HEAP_SIZE / PAGE_SIZE) / BITS_PER_WORD

bitmap_t allocated_pages[ALLOC_BITMAP_SIZE];
uint64_t heap_end_page;
uint64_t heap_start_page;
uint64_t max_pages;

void alloc_init() {
    memset(allocated_pages, 0, ALLOC_BITMAP_SIZE);
    heap_end_page = page_containing_address(HEAP_START + HEAP_SIZE - 1);
    heap_start_page = page_containing_address(HEAP_START);
    max_pages = heap_end_page - heap_start_page + 1;
}

int liballoc_lock() {
    // not implemented
    return 0;
}

int liballoc_unlock() {
    // not implemented
    return 0;
}

void* liballoc_alloc(uint32_t number_of_pages) {
    uint64_t first_free_page = 0;
    uint32_t free_page_count = 0;

   for (uint64_t i = 1; i <= max_pages; i++) {
        if (bitmap_get(allocated_pages, i) == false) {
            free_page_count++;

            if (free_page_count == number_of_pages) {
                first_free_page = i - (free_page_count - 1);
                break;
            }
        } else {
            free_page_count = 0;
        }
    }

    if (first_free_page == 0) {
        PANIC("no free pages for alloc of %d pages", number_of_pages);
    }

    uint64_t addr = page_start_address(heap_start_page + first_free_page);

    for (uint32_t i = 0; i < number_of_pages; i++) {
        bitmap_set(allocated_pages, first_free_page + i);
    }

    map_multiple(heap_start_page + first_free_page, number_of_pages,
                 PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);

    return (void*) addr;
}

int liballoc_free(void* ptr, uint32_t number_of_pages) {
    uint64_t page = page_containing_address((uint64_t)ptr);

    for (uint32_t i = 0; i < number_of_pages; i++) {
        bitmap_clear(allocated_pages, i);
    }
    
    unmap_multiple(page, number_of_pages);

    return 0;
}