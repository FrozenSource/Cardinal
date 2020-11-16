#pragma once

#include <std/types.h>
#include <drivers/driver.h>
#include <core/memory/memory.h>
#include <std/string.h>
#include <std/convert.h>

enum eMemoryType : uint32_t {
	AVAILABLE = 1,
	RESERVED = 2
};

struct tDevice {
    uint32_t uiDeviceId;
    uint32_t uiSlice;
    uint32_t uiPart;
};

struct tMemorySection {
    uint64_t pAddress;
    uint64_t ulLength;
    eMemoryType eType;
    uint32_t uiAttributes;
};

struct tELFSection {
    uint32_t eType;
    uint32_t uiFlags;
    uint64_t pAddress;
    uint64_t ulSize;
};

/* This should be in %eax. */
#define MULTIBOOT2_MAGIC_VALUE		0x36d76289

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

class cSystemInformationProvider : public IModule {
public:
	bool Init(uint64_t ulMagic, uint64_t ulMBIBegin);

	uint64_t GetTotalMemorySize() { return this->pulTotalMemorySize; };

	tDevice& GetDevice(uint8_t uiIndex) { return this->paDevices[uiIndex]; }
	uint8_t GetDeviceCount() { return this->puiDeviceCount; };
	
	tMemorySection& GetMemorySection(uint8_t uiIndex) { return this->paMemorySections[uiIndex]; }
	uint8_t GetMemorySectionCount() { return this->puiMemorySectionCount; };
	
	tELFSection& GetELFSection(uint16_t uiIndex) { return this->paELFSections[uiIndex]; }
	uint16_t GetELFSectionCount() { return this->puiELFSectionCount; };

	uint64_t GetKernelSize() { return this->pulKernelEnd - this->pulKernelStart; };
	byte* GetKernelBegin() { return (byte*) this->pulKernelStart; };
	byte* GetKernelEnd() { return (byte*) this->pulKernelEnd; };
	byte* GetBootSectionBegin() { return (byte*) this->pulMBIBegin; };
	byte* GetBootSectionEnd() { return (byte*) this->pulMBIEnd; };

	cstring GetBootLoaderName() { return this->psBootLoaderName; }
	cstring GetCommandLine() { return this->psBootCommandLine; }

protected:
	uint64_t pulMagic, pulMBIBegin, pulMBIEnd;
	uint64_t pulKernelStart, pulKernelEnd;
	uint64_t pulTotalMemorySize;

	cstring psBootLoaderName = nullptr;
	cstring psBootCommandLine = nullptr;

	tDevice paDevices[256] { 0 };
	uint8_t puiDeviceCount = 0;

	tMemorySection paMemorySections[256] { 0 };
	uint8_t puiMemorySectionCount = 0;

	tELFSection paELFSections[512] { 0 };
	uint16_t puiELFSectionCount = 0;
private:
	bool Init();
	bool Destroy();

	SINGLETON(cSystemInformationProvider)
};
