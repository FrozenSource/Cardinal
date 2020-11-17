#pragma once

#include <std/types.h>
#include <core/memory/alloc.h>

/** \defgroup ALLOCHOOKS liballoc hooks
 *
 * These are the OS specific functions which need to
 * be implemented on any platform that the library
 * is expected to work on.
 */

/** @{ */

// If we are told to not define our own size_t, then we skip the define.
#ifndef _ALLOC_SKIP_DEFINE

#ifndef _HAVE_UINTPTR_T
#define _HAVE_UINTPTR_T
typedef unsigned long long  uintptr_t;
#endif

#ifndef NULL
#define NULL        0
#endif

#endif

#define PREFIX(func)        func

void*     PREFIX(malloc)(size_t);               ///< The standard function.
void*     PREFIX(realloc)(void*, size_t);       ///< The standard function.
void*     PREFIX(calloc)(size_t, size_t);       ///< The standard function.
void      PREFIX(free)(void*);                  ///< The standard function.
