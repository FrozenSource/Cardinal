#include <core/boot.h>
#include <std/io.h>

/* The magic field should contain this. */
#define MULTIBOOT2_HEADER_MAGIC			0xe85250d6

/* This should be in %eax. */
#define MULTIBOOT2_BOOTLOADER_MAGIC		0x36d76289

/* ELF defs. */
#define MULTIBOOT_ELF_SECTION_TYPE_NULL 0

/* Flags set in the 'flags' member of the multiboot header.  */
#define MULTIBOOT_TAG_TYPE_END               0
#define MULTIBOOT_TAG_TYPE_CMDLINE           1
#define MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME  2
#define MULTIBOOT_TAG_TYPE_MODULE            3
#define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO     4
#define MULTIBOOT_TAG_TYPE_BOOTDEV           5
#define MULTIBOOT_TAG_TYPE_MMAP              6
#define MULTIBOOT_TAG_TYPE_VBE               7
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER       8
#define MULTIBOOT_TAG_TYPE_ELF_SECTIONS      9
#define MULTIBOOT_TAG_TYPE_APM               10
#define MULTIBOOT_TAG_TYPE_EFI32             11
#define MULTIBOOT_TAG_TYPE_EFI64             12
#define MULTIBOOT_TAG_TYPE_SMBIOS            13
#define MULTIBOOT_TAG_TYPE_ACPI_OLD          14
#define MULTIBOOT_TAG_TYPE_ACPI_NEW          15
#define MULTIBOOT_TAG_TYPE_NETWORK           16
#define MULTIBOOT_TAG_TYPE_EFI_MMAP          17
#define MULTIBOOT_TAG_TYPE_EFI_BS            18
#define MULTIBOOT_TAG_TYPE_EFI32_IH          19
#define MULTIBOOT_TAG_TYPE_EFI64_IH          20
#define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR    21

struct multiboot_header_t
{
    uint32_t magic;
    uint32_t architecture;
    uint32_t header_length;
    uint32_t checksum;
} PACKED;

struct multiboot_header_tag_t {
  uint16_t type;
  uint16_t flags;
  uint32_t size;
} PACKED;

struct multiboot_tag_t {
    uint32_t type;
    uint32_t size;
} PACKED;

struct multiboot_tag_string_t {
    uint32_t type;
    uint32_t size;
    char string[];
} PACKED;

struct multiboot_tag_bootdev_t {
    uint32_t type;
    uint32_t size;
    uint32_t biosdev;
    uint32_t slice;
    uint32_t part;
} PACKED;

struct multiboot_mmap_entry_t {
    uint64_t addr;
    uint64_t len;
    eMemoryType type;
    uint32_t zero;
} PACKED;

struct multiboot_tag_mmap_t {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    multiboot_mmap_entry_t entries[];
} PACKED;

struct multiboot_elf_sections_entry_t {
    uint32_t name;
    uint32_t type;
    uint64_t flags;
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t alignment;
    uint64_t entry_size;
} PACKED;

struct multiboot_tag_elf_sections_t {
    uint32_t type;
    uint32_t size;
    uint32_t num;
    uint32_t section_size;
    uint32_t shndx;
    multiboot_elf_sections_entry_t entries[];
} PACKED;

bool cSystemInformationProvider::Init(uint64_t ulMagic, uint64_t ulMBIBegin) {
    this->pulMagic = ulMagic;
    this->pulMBIBegin = ulMBIBegin;

    // Check the magic number for multiboot.
    if (this->pulMagic != MULTIBOOT2_BOOTLOADER_MAGIC) {
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
