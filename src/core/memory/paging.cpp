#include <core/memory/paging.h>

void zero_table(page_table_t* table);
page_table_t* next_table_address(page_table_t* table, uint64_t index);
uint64_t translate(uint64_t virtual_address);
opt_uint64_t pointed_frame(page_entry_t entry);
uint64_t p4_index(uint64_t page);
uint64_t p3_index(uint64_t page);
uint64_t p2_index(uint64_t page);
uint64_t p1_index(uint64_t page);
page_table_t* next_table_create(page_table_t* table, uint64_t index);
page_table_t* get_p4();
void paging_set_entry(page_entry_t* entry, uint64_t addr, uint64_t flags);
uint64_t read_cr3();
void write_cr3(uint64_t value);

void paging_init() {
    uint64_t addr1 = (uint64_t)42 * PAGE_ENTRIES * PAGE_ENTRIES * PAGE_SIZE;
    uint64_t addr2 = (uint64_t)43 * PAGE_ENTRIES * PAGE_ENTRIES * PAGE_SIZE;
    uint64_t addr3 = addr2 + PAGE_SIZE;
    uint64_t addr4 = addr3 + PAGE_SIZE;

    uint64_t page1 = page_containing_address(addr1);
    uint64_t page2 = page_containing_address(addr2);
    uint64_t page3 = page_containing_address(addr3);
    uint64_t page4 = page_containing_address(addr4);
    opt_uint64_t frame1 = frame_allocate();
    opt_uint64_t frame2 = frame_allocate();

    uint64_t* m1 = (uint64_t*)page_start_address(page1);
    uint64_t* m2 = (uint64_t*)page_start_address(page2);
    uint64_t* m3 = (uint64_t*)page_start_address(page3);
    uint64_t* m4 = (uint64_t*)page_start_address(page4);

    map_page_to_frame(page1, frame1.value, PAGING_FLAG_WRITABLE);
    map_page_to_frame(page2, frame1.value, PAGING_FLAG_WRITABLE);
    map_page_to_frame(page3, frame1.value, 0x0);
    map_page_to_frame(page4, frame2.value, 0x0);

    *m2 = 0xdeadbeef;

    if (*m1 != *m3) {
        PANIC(
            "paging test failed m1=%p m2=%p m3=%p m4=%p "
            "*m1=%x *m2=%x *m3=%x *m4=%x",
            m1, m2, m3, m4, *m1, *m2, *m3, *m4
        );
    }

    unmap(page1);
    unmap(page2);
    unmap(page3);
    unmap(page4);
}

void zero_table(page_table_t* table) {
    memset(table, 0, sizeof(page_table_t));
}

page_table_t* next_table_address(page_table_t* table, uint64_t index) {
    if (table->entries[index].present != 1) {
        DEBUG("entry at index=%u in table=%p is not present, returning 0", index, table);
        return 0;
    }

    if (table->entries[index].huge_page == 1) {
        DEBUG("huge page detected for table=%p index=%u, returning 0", table, index);
        return 0;
    }

    page_table_t* next_table = (page_table_t*)(((uint64_t)table << 9) | (index << 12));

    return next_table;
}

// Translates a virtual address to the corresponding physical address.
uint64_t translate(uint64_t virtual_address) {
    uint64_t offset = virtual_address % PAGE_SIZE;
    uint64_t page = page_containing_address(virtual_address);
    opt_uint64_t frame = translate_page(page);

    return frame.value * PAGE_SIZE + offset;
}

uint64_t page_containing_address(uint64_t virtual_address) {
    // So the address space is split into two halves: the higher half
    // containing addresses with sign extension and the lower half containing
    // addresses without. Everything in between is invalid.
    if (virtual_address < 0x0000800000000000 || virtual_address >= 0xffff800000000000) {
        return virtual_address / PAGE_SIZE;
    }

    PANIC("invalid address=%#x", virtual_address);
    return 0; // never reached but avoid a GCC warning.
}

uint64_t page_start_address(uint64_t page_number) {
    return page_number * PAGE_SIZE;
}

opt_uint64_t translate_page(uint64_t page_number) {
    page_table_t* p4 = get_p4();

    page_table_t* p3 = next_table_address(p4, p4_index(page_number));

    if (p4->entries[p4_index(page_number)].huge_page) {
        opt_uint64_t frame = pointed_frame(p3->entries[p3_index(page_number)]);
        DEBUG("1GB huge page=%u frame=%u", page_number, frame.value);

        if (frame.value % (PAGE_ENTRIES * PAGE_ENTRIES) == 0) {
            frame.value += p2_index(page_number) * PAGE_ENTRIES + p1_index(page_number);
            return frame;
        } else {
            PANIC("misaligned 1GB page=%u", page_number);
        }
    }

    if (p3 == 0) {
        DEBUG("did not find p3 (%p), returning 0", p3);
        return (opt_uint64_t){ .has_value = false, .value = 0 };
    }

    page_table_t* p2 = next_table_address(p3, p3_index(page_number));

    if (p3->entries[p3_index(page_number)].huge_page) {
        opt_uint64_t frame = pointed_frame(p2->entries[p2_index(page_number)]);
        DEBUG("2MB huge page=%u frame=%u", page_number, frame.value);

        if (frame.value % PAGE_ENTRIES == 0) {
            frame.value += p1_index(page_number);

            return frame;
        } else {
            PANIC("misaligned 2MB page=%u", page_number);
        }
    }

    if (p2 == 0) {
        DEBUG("did not find p2 (%p), returning 0", p2);
        return (opt_uint64_t){ .has_value = false, .value = 0 };
    }

    page_table_t* p1 = next_table_address(p2, p2_index(page_number));

    if (p1 == 0) {
        DEBUG("did not find p1 (%p), returning 0", p1);
        return (opt_uint64_t){ .has_value = false, .value = 0 };
    }

    return pointed_frame(p1->entries[p1_index(page_number)]);
}

uint64_t p4_index(uint64_t page) {
    return (page >> 27) & 0777;
}

uint64_t p3_index(uint64_t page) {
    return (page >> 18) & 0777;
}

uint64_t p2_index(uint64_t page) {
    return (page >> 9) & 0777;
}

uint64_t p1_index(uint64_t page) {
    return (page >> 0) & 0777;
}

opt_uint64_t pointed_frame(page_entry_t entry) {
    if (entry.present) {
        return (opt_uint64_t){ .has_value = true,
                           .value = frame_containing_address(entry.addr) };
    }

    return (opt_uint64_t){ .has_value = false, .value = 0 };
}

void map_page_to_frame(uint64_t page, uint64_t frame, uint64_t flags) {
    uint64_t frame_number = frame_containing_address(frame);

    opt_uint64_t maybe_frame = translate_page(page);
    if (maybe_frame.has_value && maybe_frame.value == frame_number) {
        DEBUG("page=%u already mapped to frame=%u", page, frame_number);
        return;
    }

    uint64_t p4_idx = p4_index(page);
    uint64_t p3_idx = p3_index(page);
    uint64_t p2_idx = p2_index(page);
    uint64_t p1_idx = p1_index(page);

    page_table_t* p3 = next_table_create(get_p4(), p4_idx);

    page_table_t* p2 = next_table_create(p3, p3_idx);

    page_table_t* p1 = next_table_create(p2, p2_idx);

    page_entry_t entry = p1->entries[p1_idx];

    if (entry.addr != 0) {
        PANIC("%s", "entry should be unused");
    }

    paging_set_entry(&entry, frame, flags | PAGING_FLAG_PRESENT);

    p1->entries[p1_idx] = entry;
}

page_table_t* next_table_create(page_table_t* table, uint64_t index) {

    bool was_created = false;

    page_entry_t entry = table->entries[index];

    if (entry.present) {
        // nothing.
    } else {
        opt_uint64_t frame = frame_allocate();

        if (!frame.has_value) {
            PANIC("%s", "frame is unexpectedly missing");
        }

        paging_set_entry(&entry, frame.value, PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);

        table->entries[index] = entry;

        was_created = true;
    }

    page_table_t* next_table = next_table_address(table, index);

    if (was_created) {
        zero_table(next_table);
    }

    return next_table;
}

void paging_set_entry(page_entry_t* entry, uint64_t addr, uint64_t flags) {
    if ((addr & !0x000ffffffffff000) != 0) {
        PANIC("trying to set an invalid address: %p", addr);
    }

    entry->addr = addr;

    if (flags & PAGING_FLAG_PRESENT) {
        entry->present = 1;
    }

    if (flags & PAGING_FLAG_WRITABLE) {
        entry->writable = 1;
    }

    if (flags & PAGING_FLAG_NO_EXECUTE) {
        entry->no_execute = 1;
    }
}

page_table_t* get_p4() {
    return (page_table_t*)P4_TABLE;
}

void map(uint64_t page_number, uint64_t flags) {
    opt_uint64_t frame = frame_allocate();

    if (!frame.has_value) {
        PANIC("out of memory", nullptr);
    }

    map_page_to_frame(page_number, frame.value, flags);
}

void unmap(uint64_t page_number) {
    uint64_t addr = page_start_address(page_number);

    if (translate(addr) == 0) {
        DEBUG("cannot unmap page=%u (start_address=%p) because it is not mapped",
          page_number,
          addr);
    }

    page_table_t* p4 = get_p4();
    page_table_t* p3 = next_table_address(p4, p4_index(page_number));
    page_table_t* p2 = next_table_address(p3, p3_index(page_number));
    page_table_t* p1 = next_table_address(p2, p2_index(page_number));

    page_entry_t entry = p1->entries[p1_index(page_number)];
    opt_uint64_t frame = pointed_frame(entry);

    memset(&p1->entries[p1_index(page_number)], 0, sizeof(page_entry_t));

    // TODO free p(1,2,3) table if empty

    frame_deallocate(frame.value);

    // flush the translation lookaside buffer
    // cf. http://os.phil-opp.com/modifying-page-tables.html#unmap
    __asm__("invlpg (%0)" : /* no output */ : "r"(addr) : "memory");
}

uint64_t read_cr3() {
    uint64_t value;

    __asm__("mov %%cr3, %0" : "=r"(value) : /* no input */);

    return value;
}

void write_cr3(uint64_t value) {
    __asm__("mov %0, %%cr3" : /* no output */ : "r"(value));
}

void map_multiple(uint64_t start_page_number, uint32_t number_of_pages, uint64_t flags) {
    for (uint32_t i = 0; i < number_of_pages; i++) {
        map(start_page_number + i, flags);
    }
}

void unmap_multiple(uint64_t start_page_number, uint32_t number_of_pages) {
    for (uint32_t i = 0; i < number_of_pages; i++) {
        unmap(start_page_number + i);
    }
}

uint32_t paging_amount_for_byte_size(uint64_t start_address, uint64_t byte_size) {

    uint64_t start_page = page_containing_address(start_address);
    uint64_t end_page = page_containing_address(start_address + byte_size);

    uint32_t difference = (uint32_t)(end_page - start_page);

    if (difference == 0) {
        return 1;
    }

    return difference;
}