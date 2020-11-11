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
