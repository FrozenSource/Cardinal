#pragma once

#include <std/types.h>
#include <std/core.h>
#include <drivers/driver.h>

class cStaticTimer : public IDriver {
public:
    bool Init();
    bool Destroy();

    uint32_t GetTick();
protected:
private:

    SINGLETON(cStaticTimer)
};