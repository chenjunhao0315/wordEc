#ifndef PLATFORM_H
#define PLATFORM_H

#include "sword_export.h"

#if SWORD_FORCE_INLINE
#ifdef _MSC_VER
    #define SWORD_FORCEINLINE __forceinline
#elif defined(__GNUC__)
    #define SWORD_FORCEINLINE inline __attribute__((__always_inline__))
#elif defined(__CLANG__)
    #if __has_attribute(__always_inline__)
        #define SWORD_FORCEINLINE inline __attribute__((__always_inline__))
    #else
        #define SWORD_FORCEINLINE inline
    #endif
#else
    #define SWORD_FORCEINLINE inline
#endif
#else
    #define SWORD_FORCEINLINE inline
#endif

#endif // PLATFORM_H