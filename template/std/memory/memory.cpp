#include "memory.h"

void* operator new(size_t n)
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
