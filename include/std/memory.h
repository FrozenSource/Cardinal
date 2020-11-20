#pragma once
#include <std/types.h>
#include <std/core.h>

C_FUNCTION void memset(void* pAddress, byte bValue, memsize_t uiSize);
C_FUNCTION void memcpy(void* pDestination, const void* pSource, memsize_t uiSize);
C_FUNCTION void memmove(void* pAddress, const void* pSource, memsize_t uiSize);
