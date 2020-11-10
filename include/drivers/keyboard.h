#pragma once

#include <std/core.h>
#include <drivers/driver.h>

class cStaticKeyBoardDriver : public IDriver {
public:
    bool Init();
    bool Destroy() { this->pbInitialized = false; return true; }

    SINGLETON(cStaticKeyBoardDriver)
};