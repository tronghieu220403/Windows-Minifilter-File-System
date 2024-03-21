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
	class FileInfoShort
	{
	private:
		PUCHAR file_info_addr_ = nullptr;
		size_t next_entry_offset_rva_ = 0;
		size_t file_name_rva_ = 0;
		size_t file_name_length_rva_ = 0;
	public:
		FileInfoShort() = default;
		FileInfoShort(PUCHAR base_va, PUCHAR next_entry_offset_va, PUCHAR file_name_va, PUCHAR file_name_length_offset_va);

		FileInfoShort(const FileInfoShort&, const PUCHAR);

		ULONG GetNextEntryOffset() const;
		PWCHAR GetFileName() const;
		ULONG GetFileNameLength() const;
		PUCHAR GetBaseAddr() const;

		ULONG Length() const;
		PUCHAR GetNextEntryAddr() const;

		void SetNextEntryOffset(const ULONG);
		void SetFileName(const PWCHAR);
		void SetFileNameLength(const ULONG);
		void SetBaseAddr(const PUCHAR);

		bool IsNull();
	};

	inline Vector<String<WCHAR>>* kHideFileList = nullptr;
	inline Vector<String<WCHAR>>* kHideDirList = nullptr;

	inline Mutex kFileMutex;
	inline Mutex kDirMutex;

	void FltRegister();

	void FltUnload();

	void DrvRegister();

	void DrvUnload();

	// NTSTATUS HideFile(PUCHAR info, PUCHAR nextEntryOffset, PUCHAR fileNameOffset, PUCHAR fileNameLengthOffset, PUCHAR info_addr);
	NTSTATUS HideFile(FileInfoShort& info);

	bool IsHiddenFile(String<WCHAR>& file_name);

	void AddFileToHideList(String<WCHAR> file_name);

	bool IsHiddenDir(String<WCHAR>& dir_name);
	void AddDirToHideList(String<WCHAR> dir_name);

	FLT_PREOP_CALLBACK_STATUS PreDirControlOperation(PFLT_CALLBACK_DATA data, PCFLT_RELATED_OBJECTS flt_objects, PVOID* completion_context);

	FLT_POSTOP_CALLBACK_STATUS PostDirControlOperation(PFLT_CALLBACK_DATA data, PCFLT_RELATED_OBJECTS flt_objects, PVOID completion_context, FLT_POST_OPERATION_FLAGS flags);
	

}