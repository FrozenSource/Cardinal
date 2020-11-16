#pragma once
#include <std/types.h>

#include <std/core.h>
#include <drivers/driver.h>

C_FUNCTION void memset(void* pAddress, byte bValue, memsize_t uiSize);
C_FUNCTION void memcpy(void* pDestination, const void* pSource, memsize_t uiSize);
C_FUNCTION void memmove(void* pAddress, const void* pSource, memsize_t uiSize);

// class cMemoryManager : public IModule {
// public:
//     bool Init();

// protected:
// private:
//     bool Destroy();

//     SINGLETON(cMemoryManager)
// };

// class cMemoryManager : public IModule {
// public:
//     bool Init();

// protected:
// private:
//     bool Destroy();

//     SINGLETON(cMemoryManager)
// };

