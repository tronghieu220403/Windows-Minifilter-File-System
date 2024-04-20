#pragma once

#include "../../std/string/string.h"
#include "../../std/vector/vector.h"
#include "../../std/sync/mutex.h"

#include "../../process/ps-monitor.h"

#include "../../template/register.h"
#include "../../template/flt-ex.h"

#include <wdm.h>
#include <Ntstrsafe.h>
#include <fltKernel.h>

namespace hide_file
{

	extern inline Vector<String<WCHAR>>* kHideFileList = nullptr;
	extern inline Vector<String<WCHAR>>* kHideDirList = nullptr;

	extern inline Mutex kFileMutex = Mutex();
	extern inline Mutex kDirMutex = Mutex();

	void FltRegister();

	void FltUnload();

	NTSTATUS HideFile(flt::FileInfoShort info, String<WCHAR>* root);

	bool IsHiddenFile(const String<WCHAR>* file_name);
	void AddFileToHideList(const String<WCHAR>* file_name);
	void DeleteFileFromHideList(const String<WCHAR>* file_name);

	bool IsHiddenDir(const String<WCHAR>* dir_name);
	void AddDirToHideList(const String<WCHAR>* dir_name);
	void DeleteDirFromHideList(const String<WCHAR>* dir_name);

	FLT_PREOP_CALLBACK_STATUS PreDirControlOperation(PFLT_CALLBACK_DATA data, PCFLT_RELATED_OBJECTS flt_objects, PVOID* completion_context);

	FLT_POSTOP_CALLBACK_STATUS PostDirControlOperation(PFLT_CALLBACK_DATA data, PCFLT_RELATED_OBJECTS flt_objects, PVOID completion_context, FLT_POST_OPERATION_FLAGS flags);
	

}