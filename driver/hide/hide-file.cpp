#include "hide-file.h"
#pragma once

void hide::FltRegister()
{
    kHideFileList = new Vector<String<WCHAR>>();
    kHideDirList = new Vector<String<WCHAR>>();
    kFileMutex.Create();
    kDirMutex.Create();
    reg::kFltFuncVector->PushBack({ IRP_MJ_DIRECTORY_CONTROL, (PFLT_PRE_OPERATION_CALLBACK)PreDirControlOperation, (PFLT_POST_OPERATION_CALLBACK)PostDirControlOperation });
    return;
}

void hide::FltUnload()
{
    delete kHideFileList;
    delete kHideDirList;
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

void hide::AddFileToHideList(String<WCHAR>& file_name)
{
	kFileMutex.Lock();
	kHideFileList->PushBack(file_name);
	kFileMutex.Unlock();
	return;
}

void hide::DeleteFileFromHideList(String<WCHAR>& file_name)
{
    kFileMutex.Lock();
    for (int i = 0; i < kHideFileList->Size(); i++)
    {
        if ((*kHideFileList)[i] == file_name)
        {
			kHideFileList->EraseUnordered(i);
			break;
		}
	}
    return;
}

bool hide::IsHiddenDir(String<WCHAR>& dir_name)
{
    bool ret = false;
    kDirMutex.Lock();
    for (int i = 0; i < kHideDirList->Size(); i++)
    {
        if ((*kHideDirList)[i] == dir_name)
        {
            ret = true;
            break;
        }
    }
    kDirMutex.Unlock();
    return ret;
}

void hide::AddDirToHideList(String<WCHAR>& dir_name)
{
    kDirMutex.Lock();
    kHideFileList->PushBack(dir_name);
    kDirMutex.Unlock();
    return;
}

void hide::DeleteDirFromHideList(String<WCHAR>& dir_name)
{
    kDirMutex.Lock();
    for (int i = 0; i < kHideDirList->Size(); i++)
    {
        if ((*kHideDirList)[i] == dir_name)
        {
			kHideDirList->EraseUnordered(i);
			break;
		}
	}
    kDirMutex.Unlock();
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

    FILE_INFORMATION_CLASS info_class;

    String<WCHAR> file_name;
    PVOID directory_buffer;
    NTSTATUS status;


    // assert we're not draining.
    ASSERT(!FlagOn(flags, FLTFL_POST_OPERATION_DRAINING));
    if (flags & FLTFL_POST_OPERATION_DRAINING)
    {
        return FLT_POSTOP_FINISHED_PROCESSING;
    }

    if (!NT_SUCCESS(data->IoStatus.Status))
        return FLT_POSTOP_FINISHED_PROCESSING;

    return FLT_POSTOP_FINISHED_PROCESSING;

    if (flt::IsTrustedRequestor(data) == true)
    {
        return FLT_POSTOP_FINISHED_PROCESSING;
    }

    if (data->Iopb->MinorFunction != IRP_MN_QUERY_DIRECTORY)
    {
        return FLT_POSTOP_FINISHED_PROCESSING;
    }

	__try
	{
		file_name = data->Iopb->Parameters.DirectoryControl.QueryDirectory.FileName;

		info_class = data->Iopb->Parameters.DirectoryControl.QueryDirectory.FileInformationClass;

		directory_buffer = MmGetSystemAddressForMdlSafe(data->Iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress, NormalPagePriority | MdlMappingNoExecute);
		if (directory_buffer == NULL)
		{
			directory_buffer = data->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer;
		}

        PUCHAR info;
        PUCHAR next_entry_offset;
        PUCHAR file_name_offset;
        PUCHAR file_name_length_offset;
        FileInfoShort file_info;

        status = STATUS_SUCCESS;

        switch (info_class)
		{
		case FileFullDirectoryInformation:
            info = (PUCHAR)directory_buffer;
            next_entry_offset = (PUCHAR) & (((PFILE_FULL_DIR_INFORMATION)directory_buffer)->NextEntryOffset);
            file_name_offset = (PUCHAR) & (((PFILE_FULL_DIR_INFORMATION)directory_buffer)->FileName);
            file_name_length_offset = (PUCHAR) & (((PFILE_FULL_DIR_INFORMATION)directory_buffer)->FileNameLength);
            file_info = FileInfoShort(info, next_entry_offset, file_name_offset, file_name_length_offset);
            status = HideFile(file_info);
            break;
		case FileBothDirectoryInformation:
            info = (PUCHAR)directory_buffer;
            next_entry_offset = (PUCHAR) & (((PFILE_BOTH_DIR_INFORMATION)directory_buffer)->NextEntryOffset);
            file_name_offset = (PUCHAR) & (((PFILE_BOTH_DIR_INFORMATION)directory_buffer)->FileName);
            file_name_length_offset = (PUCHAR) & (((PFILE_BOTH_DIR_INFORMATION)directory_buffer)->FileNameLength);
            file_info = FileInfoShort(info, next_entry_offset, file_name_offset, file_name_length_offset);
            status = HideFile(file_info);
            break;
		case FileDirectoryInformation:
            info = (PUCHAR)directory_buffer;
            next_entry_offset = (PUCHAR) & (((PFILE_DIRECTORY_INFORMATION)directory_buffer)->NextEntryOffset);
            file_name_offset = (PUCHAR) & (((PFILE_DIRECTORY_INFORMATION)directory_buffer)->FileName);
            file_name_length_offset = (PUCHAR) & (((PFILE_DIRECTORY_INFORMATION)directory_buffer)->FileNameLength);
            file_info = FileInfoShort(info, next_entry_offset, file_name_offset, file_name_length_offset);
            status = HideFile(file_info);
            break;
		case FileIdFullDirectoryInformation:
            info = (PUCHAR)directory_buffer;
            next_entry_offset = (PUCHAR) & (((PFILE_ID_FULL_DIR_INFORMATION)directory_buffer)->NextEntryOffset);
            file_name_offset = (PUCHAR) & (((PFILE_ID_FULL_DIR_INFORMATION)directory_buffer)->FileName);
            file_name_length_offset = (PUCHAR) & (((PFILE_ID_FULL_DIR_INFORMATION)directory_buffer)->FileNameLength);
            file_info = FileInfoShort(info, next_entry_offset, file_name_offset, file_name_length_offset);
            status = HideFile(file_info);
            break;
		case FileIdBothDirectoryInformation:
            info = (PUCHAR)directory_buffer;
            next_entry_offset = (PUCHAR) & (((PFILE_ID_FULL_DIR_INFORMATION)directory_buffer)->NextEntryOffset);
            file_name_offset = (PUCHAR) & (((PFILE_ID_FULL_DIR_INFORMATION)directory_buffer)->FileName);
            file_name_length_offset = (PUCHAR) & (((PFILE_ID_FULL_DIR_INFORMATION)directory_buffer)->FileNameLength);
            file_info = FileInfoShort(info, next_entry_offset, file_name_offset, file_name_length_offset);
            status = HideFile(file_info);
            break;
		case FileNamesInformation:
            info = (PUCHAR)directory_buffer;
            next_entry_offset = (PUCHAR) & (((PFILE_NAMES_INFORMATION)directory_buffer)->NextEntryOffset);
            file_name_offset = (PUCHAR) & (((PFILE_NAMES_INFORMATION)directory_buffer)->FileName);
            file_name_length_offset = (PUCHAR) & (((PFILE_NAMES_INFORMATION)directory_buffer)->FileNameLength);
            file_info = FileInfoShort(info, next_entry_offset, file_name_offset, file_name_length_offset);
            status = HideFile(file_info);
            break;
		default:
            break;
		}

        data->IoStatus.Status = status;
	}
	__finally
	{

	}
	return FLT_POSTOP_FINISHED_PROCESSING;
}

// NTSTATUS hide::HideFile(PUCHAR info, PUCHAR nextEntryOffset, PUCHAR fileNameOffset, PUCHAR fileNameLengthOffset, PUCHAR info_addr)
NTSTATUS hide::HideFile(FileInfoShort& info)
{
    FileInfoShort prev_info;
    FileInfoShort next_info;

    bool set_prev;

    if (!info.IsNull())
    {
        while (true)
        {
            set_prev = true;
            String<WCHAR> file_name_str(UNICODE_STRING{(USHORT)info.GetFileNameLength(), (USHORT)info.GetFileNameLength(), (PWCHAR)info.GetFileName()});
            size_t offset = 0;
            size_t move_length = 0;

            if (IsHiddenFile(file_name_str) == true)
            {
                if (!prev_info.IsNull())
                {
                    if (info.GetNextEntryOffset() != 0)
                    {
                        prev_info.SetNextEntryOffset(prev_info.GetNextEntryOffset() + info.GetNextEntryOffset());
                        RtlFillMemory(info.GetBaseAddr(), info.Length(), 0);
                    }
                    else
                    {
                        prev_info.SetNextEntryOffset(0);
                        RtlFillMemory(info.GetBaseAddr(), info.Length(), 0);
                        return STATUS_SUCCESS;
                    }
                }
                else
                {
                    set_prev = false;
                    if (info.GetNextEntryOffset() != 0)
                    {
                        next_info = FileInfoShort(info, info.GetNextEntryAddr());
                        move_length = 0;
                        while (next_info.GetNextEntryOffset() != 0)
                        {
                            move_length += next_info.GetNextEntryOffset();
                            next_info = FileInfoShort(info, info.GetNextEntryAddr());
                        }
                        move_length += next_info.Length();
                        RtlMoveMemory(info.GetBaseAddr(), info.GetNextEntryAddr(), move_length);//continue
                    }
                    else
                    {
                        return STATUS_NO_MORE_ENTRIES;
                    }
                }
            }

            if (info.GetNextEntryOffset() == NULL)
            {
                break;
            }
            else
            {
                if (set_prev == true)
                {
                    prev_info = info;
                }
                info = FileInfoShort(info, info.GetNextEntryAddr());
            }
        }
    }

    return STATUS_SUCCESS;
}


hide::FileInfoShort::FileInfoShort(PUCHAR base_va, PUCHAR next_entry_offset_va, PUCHAR file_name_va, PUCHAR file_name_length_offset_va)
	:file_info_addr_(base_va), next_entry_offset_rva_(next_entry_offset_va - base_va), file_name_rva_(file_name_va - base_va), file_name_length_rva_(file_name_length_offset_va - base_va)
{
	
}

hide::FileInfoShort::FileInfoShort(const FileInfoShort& file_info, const PUCHAR file_info_addr)
{
    file_info_addr_ = file_info_addr;
	next_entry_offset_rva_ = file_info.next_entry_offset_rva_;
	file_name_rva_ = file_info.file_name_rva_;
	file_name_length_rva_ = file_info.file_name_length_rva_;
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

PUCHAR hide::FileInfoShort::GetBaseAddr() const
{
    return file_info_addr_;
}

ULONG hide::FileInfoShort::Length() const
{
    return (ULONG)file_name_rva_ + GetFileNameLength();
}

PUCHAR hide::FileInfoShort::GetNextEntryAddr() const
{
    return GetBaseAddr() + GetNextEntryOffset();
}

void hide::FileInfoShort::SetNextEntryOffset(const ULONG next_entry_val)
{
    *(ULONG*)((PUCHAR)file_info_addr_ + next_entry_offset_rva_) = next_entry_val;
}

void hide::FileInfoShort::SetFileName(PWCHAR file_name)
{
    *(PWCHAR*)((PUCHAR)file_info_addr_ + file_name_length_rva_) = file_name;
}

void hide::FileInfoShort::SetFileNameLength(ULONG length)
{
    *(ULONG*)((PUCHAR)file_info_addr_ + file_name_length_rva_) = length;
}

void hide::FileInfoShort::SetBaseAddr(const PUCHAR file_info_addr)
{
	file_info_addr_ = file_info_addr;
}

bool hide::FileInfoShort::IsNull() const
{
    if (file_info_addr_ == nullptr || (next_entry_offset_rva_ == file_name_length_rva_ || file_name_length_rva_ == file_name_length_rva_ || file_name_length_rva_ == next_entry_offset_rva_))
    {
        return true;
    }
    return false;
}
