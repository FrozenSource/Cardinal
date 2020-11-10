#pragma once

#define C_FUNCTION extern "C"

extern "C" inline void __cxa_pure_virtual() {
    // Do nothing or print an error message.
}

#define SINGLETON(ClassName) \
public: \
    static ClassName& Get() { \
        static ClassName oInstance = {}; \
        return oInstance; \
    } \
private: \
    ClassName()                                     = default; \
    ~ClassName()                                    = default; \
    ClassName(ClassName const&)                     = delete; \
    ClassName(ClassName&&)                          = delete; \
    void operator=(ClassName const&)                = delete;

#define PACKED __attribute__((__packed__))
