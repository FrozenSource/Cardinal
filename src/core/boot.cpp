#include <core/boot.h>
#include <std/io.h>

bool cSystemInformationProvider::Init(uint64_t ulMagic, uint64_t ulMBIBegin) {
    this->pulMagic = ulMagic;
    this->pulMBIBegin = ulMBIBegin;

    // Check the magic number for multiboot.
    if (this->pulMagic != MULTIBOOT2_MAGIC_VALUE) {
        printf("Invalid magic number: 0x%x\n", this->pulMagic);
        return false;
    }
    if (this->pulMBIBegin & 7) {
        printf("Misaligned MBI: 0x%x\n", this->pulMBIBegin);
        return false;
    }

    multiboot_tag_t *pTag = (multiboot_tag_t *) (this->pulMBIBegin + 8);
    do {
        switch (pTag->type) {
            case MULTIBOOT_TAG_TYPE_CMDLINE:
                this->psBootCommandLine = ((multiboot_tag_string_t *) pTag)->string;
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                this->psBootLoaderName = ((multiboot_tag_string_t *) pTag)->string;
                break;
            case MULTIBOOT_TAG_TYPE_BOOTDEV:
                this->paDevices[this->puiDeviceCount].uiDeviceId = ((multiboot_tag_bootdev_t *) pTag)->biosdev;
                this->paDevices[this->puiDeviceCount].uiSlice = ((multiboot_tag_bootdev_t *) pTag)->slice;
                this->paDevices[this->puiDeviceCount].uiPart = ((multiboot_tag_bootdev_t *) pTag)->part;
                this->puiDeviceCount++;
                break;
            case MULTIBOOT_TAG_TYPE_MMAP:
                {
                    multiboot_mmap_entry_t *mmap;
                    for (
                        mmap = ((multiboot_tag_mmap_t *) pTag)->entries;
                        (uint8_t *) mmap < (uint8_t *) pTag + pTag->size;
                        mmap = (multiboot_mmap_entry_t *) ((unsigned long) mmap + ((multiboot_tag_mmap_t *) pTag)->entry_size)
                    ) {
                        this->paMemorySections[this->puiMemorySectionCount].pAddress = mmap->addr;
                        this->paMemorySections[this->puiMemorySectionCount].ulLength = mmap->len;
                        this->paMemorySections[this->puiMemorySectionCount].eType = mmap->type;
                        this->paMemorySections[this->puiMemorySectionCount].uiAttributes = mmap->zero;
                        this->puiMemorySectionCount++;
                        this->pulTotalMemorySize += mmap->len;
                    }
                }
                break;
            case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
                {
                    multiboot_elf_sections_entry_t *elf;
                    uint32_t uiIndex;
                    for (
                        uiIndex = 0,
                        elf = ((multiboot_tag_elf_sections_t *) pTag)->entries;
                        uiIndex < ((multiboot_tag_elf_sections_t *) pTag)->num;
                        elf = (multiboot_elf_sections_entry_t *) ((uint64_t) elf + ((multiboot_tag_elf_sections_t *) pTag)->section_size),
                        uiIndex++
                    ) {
                        this->paELFSections[this->puiELFSectionCount].pAddress = elf->addr;
                        this->paELFSections[this->puiELFSectionCount].eType = elf->type;
                        this->paELFSections[this->puiELFSectionCount].ulSize = elf->size;
                        this->paELFSections[this->puiELFSectionCount].uiFlags = elf->flags;
                        this->puiELFSectionCount++;

                         if (elf->type == MULTIBOOT_ELF_SECTION_TYPE_NULL) {
                            continue;
                        }

                        if (((uint64_t) (elf->addr)) < this->pulKernelStart) {
                            this->pulKernelStart = (uint64_t) elf->addr;
                        }

                        if (((uint64_t) (elf->addr)) + elf->size > this->pulKernelEnd) {
                            this->pulKernelEnd = (uint64_t) elf->addr;
                            this->pulKernelEnd += elf->size;
                        }
                    }
                }
                break;
        }
        
        pTag = (multiboot_tag_t *) ((uint8_t *) pTag + ((pTag->size + 7) & ~7));
    } while (pTag->type != MULTIBOOT_TAG_TYPE_END);

    pTag = (multiboot_tag_t *) ((uint8_t *) pTag + ((pTag->size + 7) & ~7));
    this->pulMBIEnd = (uint64_t) pTag;

    return Init();
}

bool cSystemInformationProvider::Init() {
    this->pbInitialized = true;
    return true;
}

bool cSystemInformationProvider::Destroy() {
    this->pbInitialized = false;
    return true;
}
