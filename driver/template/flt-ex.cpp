#pragma once

#include "flt-ex.h"

namespace flt
{
	bool IsTrustedRequestor(PFLT_CALLBACK_DATA data)
	{
		if (data->RequestorMode == KernelMode)
		{
			return true;
		}

		if (process::IsTrustedProcess(process::GetProcessImageName((size_t)PsGetProcessId(IoThreadToProcess(data->Thread)))) == true)
		{
			return true;
		}

		return false;
	}

	String<char> DebugIrpFlags(ULONG flag)
	{
		String<char> str;
		if (FlagOn(flag, IRP_NOCACHE))
		{
			str += String<char>("IRP_NOCACHE");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_PAGING_IO))
		{
			str += String<char>("IRP_PAGING_IO");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_MOUNT_COMPLETION))
		{
			str += String<char>("IRP_MOUNT_COMPLETION");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_SYNCHRONOUS_API))
		{
			str += String<char>("IRP_SYNCHRONOUS_API");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_ASSOCIATED_IRP))
		{
			str += String<char>("IRP_ASSOCIATED_IRP");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_BUFFERED_IO))
		{
			str += String<char>("IRP_BUFFERED_IO");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_DEALLOCATE_BUFFER))
		{
			str += String<char>("IRP_DEALLOCATE_BUFFER");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_INPUT_OPERATION))
		{
			str += String<char>("IRP_INPUT_OPERATION");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_SYNCHRONOUS_PAGING_IO))
		{
			str += String<char>("IRP_SYNCHRONOUS_PAGING_IO");
			str += String<char>(", ");
		}

		if (FlagOn(flag, IRP_CREATE_OPERATION))
		{
			str += String<char>("IRP_CREATE_OPERATION");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_READ_OPERATION))
		{
			str += String<char>("IRP_READ_OPERATION");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_WRITE_OPERATION))
		{
			str += String<char>("IRP_WRITE_OPERATION");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_CLOSE_OPERATION))
		{
			str += String<char>("IRP_CLOSE_OPERATION");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_DEFER_IO_COMPLETION))
		{
			str += String<char>("IRP_DEFER_IO_COMPLETION");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_OB_QUERY_NAME))
		{
			str += String<char>("IRP_OB_QUERY_NAME");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_HOLD_DEVICE_QUEUE))
		{
			str += String<char>("IRP_HOLD_DEVICE_QUEUE");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_UM_DRIVER_INITIATED_IO))
		{
			str += String<char>("IRP_UM_DRIVER_INITIATED_IO");
			str += String<char>(", ");
		}

		return str;
	}

	String<WCHAR> GetFileFullPathName(PFLT_CALLBACK_DATA data)
	{
		if (data == nullptr)
		{
			return String<WCHAR>();
		}
		/*
		* FltGetFileNameInformation cannot get file name information if the TopLevelIrp field of the current thread is not NULL, because the resulting file system recursion could cause deadlocks or stack overflows. (For more information about this issue, see IoGetTopLevelIrp.)
		* FltGetFileNameInformation cannot get file name information in the paging I/O path.
		* FltGetFileNameInformation cannot get file name information in the post-close path.
		* FltGetFileNameInformation cannot get the short name of a file in the pre-create path.
		*/
		if (FlagOn(data->Iopb->IrpFlags, IRP_PAGING_IO) || FlagOn(data->Iopb->IrpFlags, IRP_SYNCHRONOUS_PAGING_IO) || IoGetTopLevelIrp()) //IRP_NOCACHE
		{
			return String<WCHAR>();
		}

		String<WCHAR> res;
		PFLT_FILE_NAME_INFORMATION file_name_info;
		NTSTATUS status = FltGetFileNameInformation(data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP, &file_name_info);
		if (status == STATUS_SUCCESS)
		{
			res = String<WCHAR>(file_name_info->Name);
			FltReleaseFileNameInformation(file_name_info);
		}
		else if (status == STATUS_FLT_NAME_CACHE_MISS && FltGetFileNameInformation(data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_FILESYSTEM_ONLY, &file_name_info) == STATUS_SUCCESS)
		{
			res = String<WCHAR>(file_name_info->Name);
			FltReleaseFileNameInformation(file_name_info);
		}
		return res;
	}

	FileInfoShort::FileInfoShort(PUCHAR base_va, size_t next_entry_offset_rva, size_t file_name_rva, size_t file_name_length_rva, size_t file_attributes_rva)
		:file_info_addr_(base_va), next_entry_offset_rva_   (next_entry_offset_rva), file_name_rva_(file_name_rva), file_name_length_rva_(file_name_length_rva), file_attributes_rva_(file_attributes_rva)
	{

	}

	FileInfoShort::FileInfoShort(PUCHAR base_va, size_t next_entry_offset_rva, size_t file_name_rva, size_t file_name_length_rva)
		: file_info_addr_(base_va), next_entry_offset_rva_(next_entry_offset_rva), file_name_rva_(file_name_rva), file_name_length_rva_(file_name_length_rva)
	{

	}


	FileInfoShort::FileInfoShort(const FileInfoShort* file_info, const PUCHAR file_info_addr)
	{
		file_info_addr_ = file_info_addr;
		next_entry_offset_rva_ = file_info->next_entry_offset_rva_;
		file_name_rva_ = file_info->file_name_rva_;
		file_name_length_rva_ = file_info->file_name_length_rva_;
		file_attributes_rva_ = file_info->file_attributes_rva_;
	}

	ULONG FileInfoShort::GetNextEntryOffset() const
	{
		return *(ULONG*)((PUCHAR)file_info_addr_ + next_entry_offset_rva_);
	}

	PWCHAR FileInfoShort::GetFileName() const
	{
		return (PWCHAR)((PUCHAR)file_info_addr_ + file_name_rva_);
	}

	ULONG FileInfoShort::GetFileNameLength() const
	{
		return *(ULONG*)((PUCHAR)file_info_addr_ + file_name_length_rva_);
	}

	ULONG FileInfoShort::GetFileAttributes() const
	{
		if (file_attributes_rva_ != -1)
		{
			return *(ULONG*)((PUCHAR)file_info_addr_ + file_attributes_rva_);
		}
		return ULONG();
	}

	PUCHAR FileInfoShort::GetBaseAddr() const
	{
		return file_info_addr_;
	}

	ULONG FileInfoShort::Length() const
	{
		return (ULONG)file_name_rva_ + GetFileNameLength();
	}

	PUCHAR FileInfoShort::GetNextEntryAddr() const
	{
		return GetBaseAddr() + GetNextEntryOffset();
	}

	void FileInfoShort::SetNextEntryOffset(const ULONG next_entry_val)
	{
		*(ULONG*)((PUCHAR)file_info_addr_ + next_entry_offset_rva_) = next_entry_val;
	}

	void FileInfoShort::SetFileName(PWCHAR file_name)
	{
		*(PWCHAR*)((PUCHAR)file_info_addr_ + file_name_rva_) = file_name;
	}

	void FileInfoShort::SetFileNameLength(ULONG length)
	{
		*(ULONG*)((PUCHAR)file_info_addr_ + file_name_length_rva_) = length;
	}

	void FileInfoShort::SetFileAttributes(ULONG file_attributes)
	{
		if (file_attributes_rva_ != -1)
		{
			*(ULONG*)((PUCHAR)file_info_addr_ + file_attributes_rva_) = file_attributes;
		}
	}

	void FileInfoShort::SetBaseAddr(const PUCHAR file_info_addr)
	{
		file_info_addr_ = file_info_addr;
	}

	bool FileInfoShort::IsNull() const
	{
		if (file_info_addr_ == nullptr || (next_entry_offset_rva_ == file_name_rva_ || file_name_rva_ == file_name_length_rva_ || file_name_length_rva_ == next_entry_offset_rva_))
		{
			return true;
		}
		return false;
	}
}