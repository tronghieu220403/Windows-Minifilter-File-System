#pragma once

#include "../std/vector/vector.h"
#include "../std/string/string.h"
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
	extern inline Vector<String<WCHAR>>* kTrustedProcessList = nullptr;

	extern inline Mutex kTrustedProcessMutex = Mutex();

	void DrvRegister();

	void DrvUnload();

	bool IsTrustedProcess(const String<WCHAR>& name);

	void AddTrustedProcess(const String<WCHAR>& name);
	void RemoveTrustedProcess(const String<WCHAR>& name);

	String<WCHAR> GetProcessImageName(size_t pid);
}