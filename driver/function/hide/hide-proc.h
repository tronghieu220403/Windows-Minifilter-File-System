#pragma once
/*
#include "../../std/string/string.h"
#include "../../std/vector/vector.h"
#include "../../std/sync/mutex.h"

#include "../../process/ps-monitor.h"

#include "../../template/register.h"
#include "../../template/flt-ex.h"

namespace hide_proc
{
	extern inline Vector<eprocess::ProcInfo>* kHideProcIdList = nullptr;
	extern inline Vector<String<WCHAR>>* kHideProcImageList = nullptr;
	inline Mutex kProcIdMutex = Mutex();
	inline Mutex kProcImageMutex = Mutex();
	
	void DrvRegister();

	void DrvUnload();

	size_t GetIndexInHiddenProcIdList(size_t pid);

	void AddProcIdToHideList(size_t pid);
	void AddPeprocessToHideList(PEPROCESS peprocess);
	void DeleteProcIdFromHideList(size_t pid);

	size_t GetIndexInHiddenProcImageList(const String<WCHAR>* image_path);

	void AddProcImageToHideList(const String<WCHAR>* image_path);
	void DeleteProcImageFromHideList(const String<WCHAR>* image_path);

	void HideOnProcessList();
	void HideOnProcessList(const String<WCHAR>* image_path);

	void ProcessNotifyCallBack(PEPROCESS, size_t, PPS_CREATE_NOTIFY_INFO);

}
*/