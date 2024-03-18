#include "hide-file.h"
#pragma once

void hide::FltRegister()
{
    kHideFileList = new Vector<String<WCHAR>>();
    kFileMutex.Create();
    reg::kFltFuncVector->PushBack({ IRP_MJ_DIRECTORY_CONTROL, (PFLT_PRE_OPERATION_CALLBACK)PreDirControlOperation, (PFLT_POST_OPERATION_CALLBACK)PostDirControlOperation });
    return;
}

void hide::FltUnload()
{
    delete kHideFileList;
}

void hide::DrvRegister()
{

}

void hide::DrvUnload()
{

}

bool hide::IsHiddenFile(String<WCHAR>& file_name)
{
	bool ret = false;
	kFileMutex.Lock();
	for (int i = 0; i < kHideFileList->Size(); i++)
	{
		if ((*kHideFileList)[i] == file_name)
		{
			ret = true;
			break;
		}
	}
	kFileMutex.Unlock();
	return ret;
}

void hide::AddFileToHideList(String<WCHAR> file_name)
{
	kFileMutex.Lock();
	kHideFileList->PushBack(file_name);
	kFileMutex.Unlock();
	return;
}

FLT_PREOP_CALLBACK_STATUS hide::PreDirControlOperation(PFLT_CALLBACK_DATA data, PCFLT_RELATED_OBJECTS flt_objects, PVOID* completion_context)
{
    UNREFERENCED_PARAMETER(data);
    UNREFERENCED_PARAMETER(flt_objects);
    UNREFERENCED_PARAMETER(completion_context);
    return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

FLT_POSTOP_CALLBACK_STATUS hide::PostDirControlOperation(PFLT_CALLBACK_DATA data, PCFLT_RELATED_OBJECTS flt_objects, PVOID completion_context, FLT_POST_OPERATION_FLAGS flags)
{
	PAGED_CODE();

	// assert we're not draining.
	ASSERT(!FlagOn(flags, FLTFL_POST_OPERATION_DRAINING));
	if (flags & FLTFL_POST_OPERATION_DRAINING)
	{
		return FLT_POSTOP_FINISHED_PROCESSING;
	}
	if (!NT_SUCCESS(data->IoStatus.Status))
		return FLT_POSTOP_FINISHED_PROCESSING;

	FILE_INFORMATION_CLASS info_class;

	String<WCHAR> file_name;
	PVOID directory_buffer;
	PVOID directory_buffer_addr;

	__try
	{
		file_name = data->Iopb->Parameters.DirectoryControl.QueryDirectory.FileName;

		info_class = data->Iopb->Parameters.DirectoryControl.QueryDirectory.FileInformationClass;

		directory_buffer = MmGetSystemAddressForMdlSafe(data->Iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress, NormalPagePriority | MdlMappingNoExecute);
		directory_buffer_addr = &data->Iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress;
		if (directory_buffer == NULL)
		{
			directory_buffer = data->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer;
			directory_buffer_addr = &data->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer;
		}

		// TODO: create common class for info class
		switch (info_class)
		{
		case FileFullDirectoryInformation:
		case FileBothDirectoryInformation:
		case FileDirectoryInformation:
		case FileIdFullDirectoryInformation:
		case FileIdBothDirectoryInformation:
		case FileNamesInformation:
		default:
			break;
		}
	}
	__finally
	{

	}
	return FLT_POSTOP_FINISHED_PROCESSING;
}

hide::FileInfoShort::FileInfoShort(PUCHAR base_va, PUCHAR next_entry_offset_va, PUCHAR file_name_va, PUCHAR file_name_length_offset_va, PUCHAR info_addr)
	:file_info_addr_(info_addr), next_entry_offset_rva_(next_entry_offset_va - base_va), file_name_rva_(file_name_va - base_va), file_name_length_rva_(file_name_length_offset_va - base_va)
{
	
}

ULONG hide::FileInfoShort::GetNextEntryOffset() const
{
	return *(ULONG*)((PUCHAR)file_info_addr_ + next_entry_offset_rva_);
}

PWCHAR hide::FileInfoShort::GetFileName() const
{
	return (PWCHAR)((PUCHAR)file_info_addr_ + file_name_rva_);
}

ULONG hide::FileInfoShort::GetFileNameLength() const
{
	return *(ULONG*)((PUCHAR)file_info_addr_ + file_name_length_rva_);
}
