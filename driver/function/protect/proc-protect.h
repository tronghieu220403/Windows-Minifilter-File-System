#pragma once

#include "../../std/string/string.h"
#include "../../std/vector/vector.h"
#include "../../std/sync/mutex.h"

#include "../../process/ps-monitor.h"

#include "../../template/register.h"
#include "../../template/flt-ex.h"

namespace protect_proc
{
	extern inline Vector<eprocess::ProcInfo>* kProtectProcIdList = nullptr;
	extern inline Vector<String<WCHAR>>* kProtectProcImageList = nullptr;
	inline Mutex kProcIdMutex = Mutex();
	inline Mutex kProcImageMutex = Mutex();

	void DrvRegister();

	void DrvUnload();

	void ProcessNotifyCallBack(PEPROCESS, size_t, PPS_CREATE_NOTIFY_INFO);

}
