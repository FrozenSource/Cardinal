#pragma once

#include <drivers/driver.h>

class cStaticKeyBoardDriver : public IDriver {
public:
    bool Init();
    bool Destroy() { this->pbInitialized = false; return true; }
    static cStaticKeyBoardDriver& Get();
protected:

private:
    static cStaticKeyBoardDriver poKeyBoard;
};