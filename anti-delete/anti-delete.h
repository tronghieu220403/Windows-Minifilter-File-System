#pragma once

#include "../std/vector/vector.h"
#include "../std/string/string.h"
#include "../std/sync/mutex.h"

#include "../template/register.h"

#include <wdm.h>
#include <fltKernel.h>


namespace anti_delete
{
	inline Vector<String<WCHAR>>* kAntiList;
	inline sync::Mutex kMutex;

	void FltRegister();

	void FltUnload();

	void DrvRegister();

	void DrvUnload();

	FLT_PREOP_CALLBACK_STATUS PreOperation(_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, _Flt_CompletionContext_Outptr_ PVOID* CompletionContext);

	String<WCHAR> GetFileFullPathName(PFLT_CALLBACK_DATA data);

	bool IsProtectedFile(String<WCHAR>& file_name);

	void AddFileToProtectedList();

};

