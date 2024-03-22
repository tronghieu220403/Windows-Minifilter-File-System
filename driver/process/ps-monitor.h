#pragma once

#include "../std/vector/vector.h"
#include "../std/sync/mutex.h"
#include "../std/map/map.hpp"


#include <wdm.h>
#include <Ntstrsafe.h>
#include <fltKernel.h>

#define SYSTEM_PROCESS_ID 4

namespace process
{
	inline Map<size_t, size_t> kProcAncestor = Map<size_t, size_t>();

	extern inline Vector<size_t>* kTrustedProcessList = nullptr;

	extern inline Mutex kProcessMutex = Mutex();

	void FltRegister();

	void FltUnload();

	void DrvRegister();

	void DrvUnload();

	bool IsTrustedProcess(size_t pid);

	void AddTrustedProcess(size_t pid);
	void RemoveTrustedProcess(size_t pid);
}