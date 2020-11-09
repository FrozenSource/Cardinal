#pragma once

#include <std/types.h>

class IModule {
public:
    virtual bool Init() = 0;

    virtual bool IsInitialized() { return this->pbInitialized; }

    virtual bool Destroy() = 0;

private:
    bool pbInitialized = false;
};

class IDriver : public IModule {
public:

private:

};

class IBufferManager : public IDriver {
public:

    virtual void Set(byte bValue, memsize_t uiIndex);
    virtual void Clear() = 0;

private:

};