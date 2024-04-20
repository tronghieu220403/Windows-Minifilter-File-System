#pragma once

#include "../process/ps-monitor.h"

#define offsetof(st, m) ((size_t)(&((st *)0)->m))

namespace flt
{
	bool IsTrustedRequestor(PFLT_CALLBACK_DATA data);

	String<char> DebugIrpFlags(ULONG flag);

	String<char> DebugIopbMajorFunc(UCHAR mj);

	String<WCHAR> GetFileFullPathName(PFLT_CALLBACK_DATA data);

	class FileInfoShort
	{
	private:
		PUCHAR file_info_addr_ = nullptr;
		size_t next_entry_offset_rva_ = 0;
		size_t file_name_rva_ = 0;
		size_t file_name_length_rva_ = 0;
		size_t file_attributes_rva_ = (size_t)-1;
	public:

		FileInfoShort() = default;

		FileInfoShort(PUCHAR base_va, size_t next_entry_offset_rva, size_t file_name_rva, size_t file_name_length_rva);

		FileInfoShort(PUCHAR base_va, size_t next_entry_offset_rva, size_t file_name_rva, size_t file_name_length_rva, size_t file_attributes_rva);

		FileInfoShort(const FileInfoShort*, const PUCHAR);

		ULONG GetNextEntryOffset() const;
		PWCHAR GetFileName() const;
		ULONG GetFileNameLength() const;
		ULONG GetFileAttributes() const;
		PUCHAR GetBaseAddr() const;

		ULONG Length() const;
		PUCHAR GetNextEntryAddr() const;

		void SetNextEntryOffset(const ULONG);
		void SetFileName(const PWCHAR);
		void SetFileNameLength(const ULONG);
		void SetFileAttributes(const ULONG);
		void SetBaseAddr(const PUCHAR);

		bool IsNull() const;
	};

}