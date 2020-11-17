#pragma once

#include <std/core.h>

#define UNUSED __attribute__((unused))

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef uint64_t size_t;
typedef size_t memsize_t;

typedef char* cstring;
typedef uint8_t byte;

struct opt_uint8_t {
    bool has_value;
    uint8_t value;
};

struct opt_uint16_t {
    bool has_value;
    uint16_t value;
};

struct opt_uint32_t {
    bool has_value;
    uint32_t value;
};

struct opt_uint64_t {
    bool has_value;
    uint64_t value;
};
