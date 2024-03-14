#include "mutex.h"

void Mutex::Create()
{
    KeInitializeGuardedMutex(&mutex_);
    return;
}

void Mutex::Lock()
{
    KeAcquireGuardedMutex(&mutex_);
}

void Mutex::Unlock()
{
    KeReleaseGuardedMutex(&mutex_);
}

bool Mutex::Trylock()
{
    return (KeTryToAcquireGuardedMutex(&mutex_) == TRUE);
}


