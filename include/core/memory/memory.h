#pragma once
#include <std/types.h>

#define nullptr 0

void memset(byte* pAddress, byte bValue, memsize_t uiSize);
void memcpy(void* pDestination, const void* pSource, memsize_t uiSize);