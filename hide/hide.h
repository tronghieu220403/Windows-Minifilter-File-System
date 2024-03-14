#pragma once

#include "../std/string/string.h"
#include "../std/vector/vector.h"
#include "../std/sync/mutex.h"

#include "../template/register.h"

#include <wdm.h>
#include <Ntstrsafe.h>
#include <fltKernel.h>

namespace hide
{
	inline Vector<String<WCHAR>>* kHideFileList = nullptr;
	inline Vector<String<WCHAR>>* kHideDirList = nullptr;

	inline Mutex kFileMutex;
	inline Mutex kDirMutex;

	void FltRegister();

	void FltUnload();

	void DrvRegister();

	void DrvUnload();

	bool IsHiddenFile(String<WCHAR>& file_name);

	void AddFileToHideList(String<WCHAR> file_name);

	bool IsHiddenDir(String<WCHAR>& dir_name);
	void AddDirToHideList(String<WCHAR> dir_name);

	FLT_PREOP_CALLBACK_STATUS PreDirControlOperation(PFLT_CALLBACK_DATA data, PCFLT_RELATED_OBJECTS flt_objects, PVOID* completion_context);

	FLT_POSTOP_CALLBACK_STATUS PostDirControlOperation(PFLT_CALLBACK_DATA data, PCFLT_RELATED_OBJECTS flt_objects, PVOID completion_context, FLT_POST_OPERATION_FLAGS flags);
	
	class FileInfoShort
	{
	private:
		PUCHAR file_info_addr_;
		size_t next_entry_offset_rva_;
		size_t file_name_rva_;
		size_t file_name_length_rva_;
	public:
		FileInfoShort(PUCHAR base_va, PUCHAR next_entry_offset_va, PUCHAR file_name_va, PUCHAR file_name_length_offset_va, PUCHAR info_addr);
		
		ULONG GetNextEntryOffset() const;
		PWCHAR GetFileName() const;
		ULONG GetFileNameLength() const;
	};

}