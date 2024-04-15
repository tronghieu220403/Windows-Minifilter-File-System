#pragma once

#include "../../std/string/string.h"
#include "../../std/vector/vector.h"
#include "../../std/sync/mutex.h"

#include "../../process/ps-monitor.h"

#include "../../template/register.h"
#include "../../template/flt-ex.h"

namespace hide_proc
{
	extern inline Vector<eprocess::ProcInfo>* kHideProcList = nullptr;

	inline Mutex kProcMutex = Mutex();
	
	void DrvRegister();

	void DrvUnload();

	bool HideProc(const eprocess::ProcInfo& info);
	bool UnhideProc(const eprocess::ProcInfo& info);

	bool IsHiddenProc(size_t pid);
	void AddProcToHideList(size_t pid);
	void DeleteProcFromHideList(size_t pid);

	void ProcessNotifyCallBack(PEPROCESS, size_t, PPS_CREATE_NOTIFY_INFO);

}
