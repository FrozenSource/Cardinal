#include <core/memory/alloc.h>
#include <core/memory/paging.h>
#include <core/memory/bitmap.h>

bitmap_t allocated_pages[HEAP_SIZE / PAGE_SIZE];
uint64_t heap_end_page;
uint64_t heap_start_page;
uint64_t max_pages;

void alloc_init() {
    heap_end_page = page_containing_address(HEAP_START + HEAP_SIZE - 1);
    heap_start_page = page_containing_address(HEAP_START);
    max_pages = heap_end_page - heap_start_page + 1;

    for (uint64_t i = 0; i < HEAP_SIZE / PAGE_SIZE; i++) {
        allocated_pages[i] = 0;
    }
}

int liballoc_lock() {
    // not implemented
    return 0;
}

int liballoc_unlock() {
    // not implemented
    return 0;
}

void* liballoc_alloc(int number_of_pages) {
    uint64_t first_free_page = 0;

   for (uint64_t i = 1; i <= max_pages; i++) {
        if (bitmap_get(allocated_pages, i) == false) {
            first_free_page = i;
            break;
        }
    }

    uint64_t addr = page_start_address(heap_start_page + first_free_page);
    memset((void*)addr, 0, (first_free_page - 1) * PAGE_SIZE);

    for (uint64_t i = 0; i < number_of_pages; i++) {
        bitmap_set(allocated_pages, first_free_page + i);
        map(heap_start_page + first_free_page + i, PAGING_FLAG_WRITABLE);
    }

    return (void*) addr;
}

int liballoc_free(void* ptr, int number_of_pages) {
    uint64_t page = page_containing_address((uint64_t)ptr);

    for (uint64_t i = 0; i < number_of_pages; i++) {
        bitmap_clear(allocated_pages, i);
        unmap(page + i);
    }

    return 0;
}