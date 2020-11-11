#pragma once

#include <std/types.h>
#include <drivers/driver.h>

class cStaticBootProvider : public IModule {
public:
	bool Init(uint64_t ulMagic, uint64_t ulAddr);
	bool IsBootOk();
	void GetBootInfo();

protected:
	uint64_t pulMagic;
	uint64_t pulAddr;

private:
	bool Init();
	bool Destroy();

	SINGLETON(cStaticBootProvider)
};
