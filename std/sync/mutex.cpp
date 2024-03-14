#include "mutex.h"

void sync::Mutex::Create()
{
    KeInitializeGuardedMutex(&mutex_);
    return;
}

void sync::Mutex::Lock()
{
    KeAcquireGuardedMutex(&mutex_);
}

void sync::Mutex::Unlock()
{
    KeReleaseGuardedMutex(&mutex_);
}

bool sync::Mutex::Trylock()
{
    return (KeTryToAcquireGuardedMutex(&mutex_) == TRUE);
}
