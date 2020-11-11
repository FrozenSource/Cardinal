#include <core/boot.h>
#include <std/io.h>

/* The magic field should contain this. */
#define MULTIBOOT2_HEADER_MAGIC			0xe85250d6

/* This should be in %eax. */
#define MULTIBOOT2_BOOTLOADER_MAGIC		0x36d76289

/* Memory defs. */
#define MULTIBOOT_MEMORY_AVAILABLE  1
#define MULTIBOOT_MEMORY_RESERVED   2

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
};

struct multiboot_header_tag_t {
  uint16_t type;
  uint16_t flags;
  uint32_t size;
};

struct multiboot_tag_t {
    uint32_t type;
    uint32_t size;
};

struct multiboot_tag_string_t {
    uint32_t type;
    uint32_t size;
    char string[];
};

struct multiboot_tag_basic_meminfo_t {
    uint32_t type;
    uint32_t size;
    uint32_t mem_lower;
    uint32_t mem_upper;
};

struct multiboot_tag_bootdev_t {
    uint32_t type;
    uint32_t size;
    uint32_t biosdev;
    uint32_t slice;
    uint32_t part;
};

struct multiboot_tag_module_t {
    uint32_t type;
    uint32_t size;
    uint32_t mod_start;
    uint32_t mod_end;
    char cmdline[];
};

struct multiboot_mmap_entry_t {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t zero;
};

struct multiboot_tag_mmap_t {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    multiboot_mmap_entry_t entries[];
};

struct multiboot_tag_network_t {
    uint32_t type;
    uint32_t size;
    uint8_t dhcpack[];
};

struct multiboot_elf_sections_entry_t {
    uint32_t type;
    uint32_t flags;
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t alignment;
    uint64_t entry_size;
};

struct multiboot_tag_elf_sections_t {
    uint32_t type;
    uint32_t size;
    uint32_t num;
    uint32_t section_size;
    uint32_t shndx;
    multiboot_elf_sections_entry_t entries[];
};

bool cStaticBootProvider::Init(uint64_t ulMagic, uint64_t ulAddr) {
    this->pulMagic = ulMagic;
    this->pulAddr = ulAddr;
    return Init();
}

bool cStaticBootProvider::Init() {
    this->pbInitialized = true;
    return true;
}

bool cStaticBootProvider::Destroy() {
    this->pbInitialized = false;
    return true;
}

bool cStaticBootProvider::IsBootOk() {
    if (!IsInitialized()) return false;

    if (this->pulMagic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        printf("Invalid magic number: 0x%x\n", this->pulMagic);
        return false;
    }

    if (this->pulAddr & 7) {
        printf("Unaligned MBI: 0x%x\n", this->pulAddr);
        return false;
    }

    return true;
}

void cStaticBootProvider::GetBootInfo() {
    multiboot_tag_t *tag;

    uint64_t ulKernelStart = -1;
    uint64_t ulKernelEnd = 0;

    printf("announced MBI size 0x%x\n", *(unsigned *) this->pulAddr);

    for (
        tag = (multiboot_tag_t *) (this->pulAddr + 8);
        tag->type != MULTIBOOT_TAG_TYPE_END;
        tag = (multiboot_tag_t *) ((uint8_t *) tag + ((tag->size + 7) & ~7))
    ) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_CMDLINE:
                printf("Command line = %s\n", ((multiboot_tag_string_t *) tag)->string);
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                printf("Boot loader name = %s\n", ((multiboot_tag_string_t *) tag)->string);
                break;
            case MULTIBOOT_TAG_TYPE_MODULE:
                printf(
                    "Module at 0x%x-0x%x. command line %s\n",
                    ((multiboot_tag_module_t *) tag)->mod_start,
                    ((multiboot_tag_module_t *) tag)->mod_end,
                    ((multiboot_tag_module_t *) tag)->cmdline
                );
                break;
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                printf(
                    "mem_lower = %dKB, mem_upper = %iKB\n",
                    ((multiboot_tag_basic_meminfo_t *) tag)->mem_lower,
                    ((multiboot_tag_basic_meminfo_t *) tag)->mem_upper
                );
                break;
            case MULTIBOOT_TAG_TYPE_BOOTDEV:
                printf(
                    "Boot device 0x%x,%u,%u\n",
                    ((multiboot_tag_bootdev_t *) tag)->biosdev,
                    ((multiboot_tag_bootdev_t *) tag)->slice,
                    ((multiboot_tag_bootdev_t *) tag)->part
                );
                break;
            case MULTIBOOT_TAG_TYPE_MMAP:
                {
                    multiboot_mmap_entry_t *mmap;
                    for (
                        mmap = ((multiboot_tag_mmap_t *) tag)->entries;
                        (uint8_t *) mmap < (uint8_t *) tag + tag->size;
                        mmap = (multiboot_mmap_entry_t *) ((unsigned long) mmap + ((multiboot_tag_mmap_t *) tag)->entry_size)
                    ) {
                        printf(
                            "mmap base_addr = 0x%x%x, length = 0x%x%x, type = 0x%x\n",
                            (unsigned) (mmap->addr >> 32),
                            (unsigned) (mmap->addr & 0xffffffff),
                            (unsigned) (mmap->len >> 32),
                            (unsigned) (mmap->len & 0xffffffff),
                            (unsigned) mmap->type
                        );
                    }
                }
                break;
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                printf("%s\n", "Framebuffer");
                break;
            case MULTIBOOT_TAG_TYPE_APM:
                printf("%s\n", "Apm");
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_OLD:
                printf("%s\n", "Acpi old");
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_NEW:
                printf("%s\n", "Acpi new");
                break;
            case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
                {
                    multiboot_elf_sections_entry_t *elf;
                    uint32_t uiIndex;
                    for (
                        uiIndex = 0,
                        elf = ((multiboot_tag_elf_sections_t *) tag)->entries;
                        uiIndex < ((multiboot_tag_elf_sections_t *) tag)->num;
                        elf = (multiboot_elf_sections_entry_t *) ((uint64_t) elf + ((multiboot_tag_elf_sections_t *) tag)->section_size),
                        uiIndex++
                    ) {
                        printf(
                            "elf section #%i addr = 0x%x, type = 0x%x, size = 0x%x, flags = 0x%x\n",
                            uiIndex,
                            (unsigned) (elf->addr >> 32),
                            (unsigned) (elf->addr & 0xffffffff),
                            elf->type,
                            (unsigned) (elf->size >> 32),
                            (unsigned) (elf->size & 0xffffffff),
                            elf->flags
                        );

                         if (elf->type == MULTIBOOT_ELF_SECTION_TYPE_NULL) {
                            continue;
                        }

                        if (((uint64_t) (elf->addr)) < ulKernelStart) {
                            ulKernelStart = (uint64_t) elf->addr;
                        }

                        if (((uint64_t) (elf->addr)) + elf->size > ulKernelEnd) {
                            ulKernelEnd = (uint64_t) elf->addr;
                            ulKernelEnd += elf->size;
                        }
                    }
                }
                break;
            case MULTIBOOT_TAG_TYPE_NETWORK:
                printf("%s\n", "Network");
                break;
            case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR:
                printf("%s\n", "Load base addr");
                break;
            default:
                printf("Tag 0x%x, size 0x%x\n", tag->type, tag->size);
        }
    }
    
    tag = (multiboot_tag_t *) ((uint8_t *) tag + ((tag->size + 7) & ~7));
    printf("Total MBI size 0x%x\n", (unsigned long) tag - this->pulAddr);

    printf("kernel start = 0x%x, kernel end = 0x%x\n", (ulKernelStart >> 32), (ulKernelEnd >> 32));
    printf("multiboot start = 0x%x, multiboot end = 0x%x\n", this->pulAddr, tag);
}