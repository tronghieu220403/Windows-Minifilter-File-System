#pragma once

#include "std/vector/vector.h"
#include "std/string/string.h"
#include "std/sync/mutex.h"

#include "process/ps-monitor.h"
#include "template/register.h"

#include <wdm.h>
#include <fltKernel.h>


namespace protection
{
	inline Vector<String<WCHAR>>* kProtectedFileList;
	inline Vector<String<WCHAR>>* kProtectedDirList;
	
	inline Mutex kFileMutex = Mutex();
	
	inline Mutex kDirMutex;

	void FltRegister();

	void FltUnload();

	void DrvRegister();

	void DrvUnload();

	bool IsProtectedFile(String<WCHAR>& file_name);
	void AddFileToProtectedList(String<WCHAR>& file_name);
	void RemoveFileFromProtectedList(String<WCHAR>& file_name);

	bool IsProtectedDir(String<WCHAR>& dir_name);
	void AddDirToProtectedList(String<WCHAR>& file_name);
	void RemoveDirFromProtectedList(String<WCHAR>& dir_name);

	FLT_PREOP_CALLBACK_STATUS PreOperation(_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, _Flt_CompletionContext_Outptr_ PVOID* CompletionContext);

	String<WCHAR> GetFileFullPathName(PFLT_CALLBACK_DATA data);


};

