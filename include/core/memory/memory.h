#pragma once
#include <std/types.h>

#include <std/core.h>
#include <drivers/driver.h>

void memset(byte* pAddress, byte bValue, memsize_t uiSize);
void memcpy(void* pDestination, const void* pSource, memsize_t uiSize);

class cMemoryManager : public IModule {
public:
    bool Init();
    bool Destroy();
protected:
private:

    SINGLETON(cMemoryManager)
};