#pragma once

#include "../../std/vector/vector.h"
#include "../../std/string/string.h"
#include "../../std/sync/mutex.h"

#include "../../process/ps-monitor.h"
#include "../../template/register.h"
#include "../../template/flt-ex.h"

#include <wdm.h>
#include <fltKernel.h>


namespace protect_file
{
	inline Vector<String<WCHAR>>* kProtectedFileList;
	inline Vector<String<WCHAR>>* kProtectedDirList;
	
	inline Mutex kFileMutex = Mutex();
	
	inline Mutex kDirMutex = Mutex();

	void FltRegister();

	void FltUnload();

	void DrvRegister();

	void DrvUnload();

	bool IsProtectedFile(const String<WCHAR>* file_name);
	void AddFileToProtectedList(const String<WCHAR>* file_name);
	void RemoveFileFromProtectedList(const String<WCHAR>* file_name);

	bool IsProtectedDir(const String<WCHAR>* dir_name);
	void AddDirToProtectedList(const String<WCHAR>* file_name);
	void RemoveDirFromProtectedList(const String<WCHAR>* dir_name);

	FLT_PREOP_CALLBACK_STATUS PreOperation(_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, _Flt_CompletionContext_Outptr_ PVOID* CompletionContext);

};

