#pragma once

#include <std/types.h>

class IModule {
public:
    virtual bool Init() = 0;

    virtual bool IsInitialized() { return this->pbInitialized; }

    virtual bool Destroy() = 0;

protected:
    bool pbInitialized = false;
};

class IDriver : public IModule {
public:

private:

};