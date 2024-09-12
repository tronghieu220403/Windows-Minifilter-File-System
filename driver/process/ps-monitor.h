#pragma once

#include "../std/vector/vector.h"
#include "../std/sync/mutex.h"
#include "../std/map/map.hpp"
#include "../std/memory/pair.h"
#include "../process/eprocess.h"
#include "../template/debug.h"

#include <wdm.h>
#include <Ntstrsafe.h>
#include <fltKernel.h>

namespace process
{
	extern inline int* ppid = nullptr;

	extern inline Vector<size_t>* kTrustedProcessList = nullptr;

	extern inline Mutex kProcessMutex = Mutex();

	void DrvRegister();

	void DrvUnload();

	bool IsTrustedProcess(size_t pid);

	void AddTrustedProcess(size_t pid);
	void RemoveTrustedProcess(size_t pid);

	String<WCHAR> GetProcessImageName(size_t pid);
}