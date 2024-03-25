#include "memory.h"

void* operator new(size_t n)
{
    void* p;
    while ((p = krnl_std::Alloc(n)) == 0);
    return (p);
}

void* operator new[](size_t n)
{
    void* p;
    while ((p = krnl_std::Alloc(n)) == 0);
    return (p);
}

void operator delete(void* p, size_t n)
{
    krnl_std::Free(p);
}

void operator delete[](void* p)
{
    if (p == nullptr)
    {
        return;
    }
    krnl_std::Free(p);
}

void operator delete[](void* p, size_t n)
{
    if (p == nullptr)
    {
        return;
    }
    krnl_std::Free(p);
}

void SetUlongAt(size_t addr, ULONG value)
{
    *(ULONG*)addr = value;
}

ULONG GetUlongAt(size_t addr)
{
    return *(ULONG*)addr;
}

size_t Rand()
{
    LARGE_INTEGER seed;
    KeQueryTickCount(&seed);
    seed.LowPart = (ULONG)(seed.LowPart ^ (size_t)PsGetCurrentThreadId() ^ (size_t)PsGetCurrentProcessId());
    seed.HighPart = (ULONG)(seed.HighPart ^ (size_t)PsGetCurrentThreadId() ^ (size_t)PsGetCurrentProcessId());
    seed.LowPart = seed.LowPart ^ (seed.LowPart << 13);
    seed.LowPart = seed.LowPart ^ (seed.LowPart >> 17);
    seed.LowPart = seed.LowPart ^ (seed.LowPart << 5);
    return seed.LowPart;
}