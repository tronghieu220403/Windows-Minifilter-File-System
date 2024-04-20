#include "hide-file.h"

namespace hide_file
{

    void FltRegister()
    {
        kHideFileList = new Vector<String<WCHAR>>();
        kHideDirList = new Vector<String<WCHAR>>();
        kFileMutex.Create();
        kDirMutex.Create();
        reg::kFltFuncVector->PushBack({ IRP_MJ_DIRECTORY_CONTROL, (PFLT_PRE_OPERATION_CALLBACK)PreDirControlOperation, (PFLT_POST_OPERATION_CALLBACK)PostDirControlOperation });
        return;
    }

    void FltUnload()
    {
        delete kHideFileList;
        delete kHideDirList;
    }

    bool IsHiddenFile(const String<WCHAR>* file_name)
    {
	    bool ret = false;
	    kFileMutex.Lock();
	    for (int i = 0; i < kHideFileList->Size(); i++)
	    {
		    if ((*kHideFileList)[i] == *file_name)
		    {
			    ret = true;
			    break;
		    }
	    }
	    kFileMutex.Unlock();
	    return ret;
    }

    void AddFileToHideList(const String<WCHAR>* file_name)
    {
	    kFileMutex.Lock();
	    kHideFileList->PushBack(*file_name);
	    kFileMutex.Unlock();
	    return;
    }

    void DeleteFileFromHideList(const String<WCHAR>* file_name)
    {
        kFileMutex.Lock();
        for (int i = 0; i < kHideFileList->Size(); i++)
        {
            if ((*kHideFileList)[i] == *file_name)
            {
			    kHideFileList->EraseUnordered(i);
			    break;
		    }
	    }
        kFileMutex.Unlock();
        return;
    }

    bool IsHiddenDir(const String<WCHAR>* dir_name)
    {
        bool ret = false;
        kDirMutex.Lock();
        for (int i = 0; i < kHideDirList->Size(); i++)
        {
            if ((*kHideDirList)[i] == *dir_name)
            {
                ret = true;
                break;
            }
        }
        kDirMutex.Unlock();
        return ret;
    }

    void AddDirToHideList(const String<WCHAR>* dir_name)
    {
        kDirMutex.Lock();
        kHideFileList->PushBack(*dir_name);
        kDirMutex.Unlock();
        return;
    }

    void DeleteDirFromHideList(const String<WCHAR>* dir_name)
    {
        kDirMutex.Lock();
        for (int i = 0; i < kHideDirList->Size(); i++)
        {
            if ((*kHideDirList)[i] == *dir_name)
            {
			    kHideDirList->EraseUnordered(i);
			    break;
		    }
	    }
        kDirMutex.Unlock();
        return;
    }

    FLT_PREOP_CALLBACK_STATUS PreDirControlOperation(PFLT_CALLBACK_DATA data, PCFLT_RELATED_OBJECTS flt_objects, PVOID* completion_context)
    {
        UNREFERENCED_PARAMETER(data);
        UNREFERENCED_PARAMETER(flt_objects);
        UNREFERENCED_PARAMETER(completion_context);
        return FLT_PREOP_SUCCESS_WITH_CALLBACK;
    }

    FLT_POSTOP_CALLBACK_STATUS PostDirControlOperation(PFLT_CALLBACK_DATA data, PCFLT_RELATED_OBJECTS flt_objects, PVOID completion_context, FLT_POST_OPERATION_FLAGS flags)
    {

        // TODO: To improve performance, precheck if the path is in the hidden list or dir so that we do not need to check in the user buffer. 
        // To do this, you need to split file name into 2 parts: path + name.
        // Warning: test with shortcut and symbolics link. If anyone can access the file, The TODO method is wrong.

	    PAGED_CODE();

        FILE_INFORMATION_CLASS info_class;

        String<WCHAR> file_name;
        PVOID bufffer;
        NTSTATUS status;

        // assert we're not draining.
        ASSERT(!FlagOn(flags, FLTFL_POST_OPERATION_DRAINING));
        if (flags & FLTFL_POST_OPERATION_DRAINING)
        {
            return FLT_POSTOP_FINISHED_PROCESSING;
        }

        if (!NT_SUCCESS(data->IoStatus.Status))
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
            DebugMessage("File name real: %ws", flt::GetFileFullPathName(data).Data());
            file_name = data->Iopb->Parameters.DirectoryControl.QueryDirectory.FileName;
            DebugMessage("File name fake: %ws", file_name.Data());
		    info_class = data->Iopb->Parameters.DirectoryControl.QueryDirectory.FileInformationClass;

            if (data->Iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress != NULL) {
                bufffer = MmGetSystemAddressForMdlSafe(data->Iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress, NormalPagePriority | MdlMappingNoExecute);
            }
            else {
                bufffer = data->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer;
            }
            if (bufffer == NULL) {
                __leave;
            }

            flt::FileInfoShort file_info;

            status = STATUS_SUCCESS;

            switch (info_class)
		    {
		    case FileFullDirectoryInformation:
                file_info = flt::FileInfoShort(
                    (PUCHAR)bufffer,
                    offsetof(FILE_FULL_DIR_INFORMATION, NextEntryOffset),
                    offsetof(FILE_FULL_DIR_INFORMATION, FileName),
                    offsetof(FILE_FULL_DIR_INFORMATION, FileNameLength),
                    offsetof(FILE_FULL_DIR_INFORMATION, FileAttributes)
                );
                status = HideFile(file_info);
                break;
		    case FileBothDirectoryInformation:
                file_info = flt::FileInfoShort(
                    (PUCHAR)bufffer,
                    offsetof(FILE_BOTH_DIR_INFORMATION, NextEntryOffset),
                    offsetof(FILE_BOTH_DIR_INFORMATION, FileName),
                    offsetof(FILE_BOTH_DIR_INFORMATION, FileNameLength),
                    offsetof(FILE_BOTH_DIR_INFORMATION, FileAttributes)
                );
                status = HideFile(file_info);
                break;
		    case FileDirectoryInformation:
                file_info = flt::FileInfoShort(
                    (PUCHAR)bufffer,
                    offsetof(FILE_DIRECTORY_INFORMATION, NextEntryOffset),
                    offsetof(FILE_DIRECTORY_INFORMATION, FileName),
                    offsetof(FILE_DIRECTORY_INFORMATION, FileNameLength),
                    offsetof(FILE_DIRECTORY_INFORMATION, FileAttributes)

                );
                status = HideFile(file_info);
                break;
		    case FileIdFullDirectoryInformation:
                file_info = flt::FileInfoShort(
                    (PUCHAR)bufffer,
                    offsetof(FILE_ID_FULL_DIR_INFORMATION, NextEntryOffset),
                    offsetof(FILE_ID_FULL_DIR_INFORMATION, FileName),
                    offsetof(FILE_ID_FULL_DIR_INFORMATION, FileNameLength),
                    offsetof(FILE_ID_FULL_DIR_INFORMATION, FileAttributes)

                );
                status = HideFile(file_info);
                break;
		    case FileIdBothDirectoryInformation:
                file_info = flt::FileInfoShort(
                    (PUCHAR)bufffer,
                    offsetof(FILE_ID_BOTH_DIR_INFORMATION, NextEntryOffset),
                    offsetof(FILE_ID_BOTH_DIR_INFORMATION, FileName),
                    offsetof(FILE_ID_BOTH_DIR_INFORMATION, FileNameLength),
                    offsetof(FILE_ID_BOTH_DIR_INFORMATION, FileAttributes)
                );
                status = HideFile(file_info);
                break;
		    case FileNamesInformation:
                file_info = flt::FileInfoShort(
                    (PUCHAR)bufffer,
                    offsetof(FILE_NAMES_INFORMATION, NextEntryOffset),
                    offsetof(FILE_NAMES_INFORMATION, FileName),
                    offsetof(FILE_NAMES_INFORMATION, FileNameLength)
                );
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

    NTSTATUS HideFile(flt::FileInfoShort info)
    {
        flt::FileInfoShort prev_info;
        flt::FileInfoShort next_info;

        bool set_prev;

        if (!info.IsNull())
        {
            while (true)
            {
                set_prev = true;
                String<WCHAR> file_name_str(UNICODE_STRING{(USHORT)info.GetFileNameLength(), (USHORT)info.GetFileNameLength(), (PWCHAR)info.GetFileName()});
                size_t offset = 0;
                size_t move_length = 0;

                if ((IsHiddenFile(&file_name_str) == true && !FlagOn(info.GetFileAttributes(), FILE_ATTRIBUTE_DIRECTORY)) || 
                    (IsHiddenDir(&file_name_str) == true && (FlagOn(info.GetFileAttributes(), FILE_ATTRIBUTE_DIRECTORY) || info.GetFileAttributes() == 0)))
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
                            next_info = flt::FileInfoShort(&info, info.GetNextEntryAddr());
                            move_length = 0;
                            while (next_info.GetNextEntryOffset() != 0)
                            {
                                move_length += next_info.GetNextEntryOffset();
                                next_info = flt::FileInfoShort(&info, info.GetNextEntryAddr());
                            }
                            move_length += next_info.Length();
                            RtlMoveMemory(info.GetBaseAddr(), info.GetNextEntryAddr(), move_length);
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
                    info = flt::FileInfoShort(&info, info.GetNextEntryAddr());
                }
            }
        }

        return STATUS_SUCCESS;
    }


    

}