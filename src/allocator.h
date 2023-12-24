#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "platform.h"
#include <stdlib.h>

#define MALLOC_ALIGN 16
#define MALLOC_OVERREAD 0

template<typename _Tp>
static SWORD_FORCEINLINE _Tp* alignPtr(_Tp* ptr, int n = (int)sizeof(_Tp)) {
    return (_Tp*)(((size_t)ptr + n - 1) & -n);
}

static SWORD_FORCEINLINE size_t alignSize(size_t sz, int n) {
    return (sz + n - 1) & -n;
}

static SWORD_FORCEINLINE void* fastMalloc(size_t size) {
#if _MSC_VER
    return _aligned_malloc(size, MALLOC_ALIGN);
#elif (defined(__unix__) || defined(__APPLE__)) && _POSIX_C_SOURCE >= 200112L
    void* ptr = 0;
    if (posix_memalign(&ptr, MALLOC_ALIGN, size + MALLOC_OVERREAD))
        ptr = 0;
    return ptr;
#else
    unsigned char* udata = (unsigned char*)malloc(size + sizeof(void*) + MALLOC_ALIGN + MALLOC_OVERREAD);
    if (!udata)
        return 0;
    unsigned char** adata = alignPtr((unsigned char**)udata + 1, MALLOC_ALIGN);
    adata[-1] = udata;
    return adata;
#endif
}

static SWORD_FORCEINLINE void fastFree(void* ptr) {
    if (ptr) {
#if _MSC_VER
        _aligned_free(ptr);
#elif (defined(__unix__) || defined(__APPLE__)) && _POSIX_C_SOURCE >= 200112L
        free(ptr);
#else
        unsigned char* udata = ((unsigned char**)ptr)[-1];
        free(udata);
#endif
    }
}

static SWORD_FORCEINLINE int XADD(int* addr, int delta) {
    int tmp = *addr;
    *addr += delta;
    return tmp;
}

#endif // ALLOCATOR_H
