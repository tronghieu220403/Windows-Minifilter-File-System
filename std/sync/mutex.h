#pragma once

#include <ntifs.h>
#include <ntdef.h>
#include <wdm.h>

class Mutex
{
private:
	KGUARDED_MUTEX mutex_ = { 0 };
public:

	void Create();

	void Lock();

	void Unlock();

	bool Trylock();
};

