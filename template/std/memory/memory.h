#pragma once

typedef unsigned long long size_t;

#include <ntifs.h>
#include <ntdef.h>
#include <wdm.h>

#pragma warning(disable:4100)

#define max(X, Y) (((X) > (Y)) ? (X) : (Y))
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

extern void* operator new(size_t);

extern void operator delete(void*, size_t);

extern void operator delete[](void*);

void MemCopy(void* dst, void* src, size_t len);

template <class T>
void ZeroMemory(T* dst, size_t len);

namespace krnl_std
{
    void* Alloc(size_t n);

    void Free(void* p);

}

inline void MemCopy(void* dst, void* src, size_t len)
{
    RtlCopyMemory(dst, src, len);
    return;
}

template <class T>
inline void ZeroMemory(T* dst, size_t len)
{
    for (int i = 0; i < len; ++i)
    {
        dst[i] = T();
    }
}

namespace krnl_std
{
    inline void* Alloc(size_t n)
    {
        void* p;

        p = ExAllocatePool2(POOL_FLAG_NON_PAGED, n, 0x22042003);
        // ZeroMemory(p, n);
        return p;
    }

    inline void Free(void* p)
    {
        if (p == nullptr)
        {
            return;
        }
        return ExFreePool(p);
    }

}
