#include "file-protect.h"

namespace protect_file
{
	void FltRegister()
	{
		kEnableProtectFile = true;
		kProtectedFileList = new Vector<String<WCHAR>>();
		kProtectedDirList = new Vector<String<WCHAR>>();
		kFileMutex.Create();
		kDirMutex.Create();
		reg::kFltFuncVector->PushBack({ IRP_MJ_DIRECTORY_CONTROL, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_SET_INFORMATION, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_CREATE, PreOperation, nullptr });
		DebugMessage("protect_file FltRegister completed successfully.");
		return;
	}

	void FltUnload()
	{
		delete kProtectedFileList;
		delete kProtectedDirList;
	}

	FLT_PREOP_CALLBACK_STATUS PreOperation(_Inout_ PFLT_CALLBACK_DATA data, _In_ PCFLT_RELATED_OBJECTS flt_objects, _Flt_CompletionContext_Outptr_ PVOID* completion_context)
	{
		UNREFERENCED_PARAMETER(completion_context);

		NTSTATUS status;

		PAGED_CODE();

		if (kEnableProtectFile == false)
		{
			return FLT_PREOP_SUCCESS_NO_CALLBACK;
		}

		if (flt::IsTrustedRequestor(data) == true)
		{
			return FLT_PREOP_SUCCESS_NO_CALLBACK;
		}

		String<WCHAR> name(flt::GetFileFullPathName(data));

		if (name.Size() == 0)
		{
			goto return_success_no_callback;
		}

		if (data->Iopb->MajorFunction != IRP_MJ_CREATE && data->Iopb->MajorFunction != IRP_MJ_SET_INFORMATION)
		{
			goto return_success_no_callback;
		}

		BOOLEAN is_dir;
		if (FlagOn(data->Iopb->Parameters.Create.Options, FILE_DIRECTORY_FILE))
		{
			is_dir = true;
		}
		else if (FlagOn(data->Iopb->Parameters.Create.Options, FILE_NON_DIRECTORY_FILE))
		{
			is_dir = false;
		}
		else
		{
			// https://community.osr.com/t/directory-detection-in-precreate-operation/47359/2
			// Step: FltCreateFileEx -> FltIsDirectory -> FltClose
			HANDLE file_handle; 
			PFILE_OBJECT file_object;
			IO_STATUS_BLOCK io_status_block;
			OBJECT_ATTRIBUTES oa;
			UNICODE_STRING uni_str = {(USHORT)(name.Size() * 2), (USHORT)(name.Size() * 2), name.Data()};
			
			InitializeObjectAttributes(&oa,
				&uni_str,
				OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
				NULL,
				NULL);
			status = FltCreateFileEx(flt_objects->Filter,
				data->Iopb->TargetInstance,
				&file_handle,
				&file_object,
				FILE_READ_ATTRIBUTES,
				&oa,
				&io_status_block,
				0,
				FILE_ATTRIBUTE_NORMAL,
				0,
				FILE_OPEN,
				FILE_COMPLETE_IF_OPLOCKED,
				NULL,
				0,
				IO_IGNORE_SHARE_ACCESS_CHECK | IO_OPEN_TARGET_DIRECTORY);
			if (status == STATUS_SUCCESS)
			{
				status = FltIsDirectory(file_object, flt_objects->Instance, &is_dir);
				FltClose(file_handle);
				if (!NT_SUCCESS(status))
				{
					DebugMessage("[+] FltIsDirectory failed with status %x for file %wS in line %d, function %s,", status, name.Data(), __LINE__, __FUNCTION__);
					DebugMessage("\n");
					goto return_success_no_callback;
				}
			}
			else
			{
				DebugMessage("[+] FltCreateFileEx failed with statux %x for file %wS in line %d, function %s,", status, name.Data(), __LINE__, __FUNCTION__);
				DebugMessage("\n");
				goto return_success_no_callback;
			}
		}

		ACCESS_MASK flag;

		if ((is_dir && IsProtectedDir(&name)) || (!is_dir && IsProtectedFile(&name)))
		{
			switch (data->Iopb->MajorFunction)
			{
			case IRP_MJ_SET_INFORMATION:
				goto return_access_denided;
			case IRP_MJ_CREATE:
				flag = data->Iopb->Parameters.Create.SecurityContext->DesiredAccess;
				ClearFlag(flag, FILE_WRITE_DATA);
				ClearFlag(flag, FILE_WRITE_ATTRIBUTES);
				ClearFlag(flag, FILE_WRITE_EA);
				ClearFlag(flag, FILE_APPEND_DATA);
				ClearFlag(flag, DELETE);
				ClearFlag(flag, WRITE_DAC);
				ClearFlag(flag, WRITE_OWNER);
				ClearFlag(flag, GENERIC_WRITE);
				ClearFlag(flag, STANDARD_RIGHTS_WRITE);
				data->Iopb->Parameters.Create.SecurityContext->DesiredAccess = flag;
				FltSetCallbackDataDirty(data);
				break;
			default:
				break;
			}

			// https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-create
			// When the system tries to open a handle to a file object,
			// detect requests that have DELETE_ON_CLOSE in DesiredAccess
			if (data->Iopb->MajorFunction == IRP_MJ_CREATE) 
			{
				if (!FlagOn(data->Iopb->Parameters.Create.Options, FILE_DELETE_ON_CLOSE)) 
				{
					goto return_access_denided;
				}
				goto return_success_no_callback;
			}
		}

		return_success_no_callback:
			return FLT_PREOP_SUCCESS_NO_CALLBACK;

		return_access_denided:
			data->IoStatus.Status = STATUS_ACCESS_DENIED;
			data->IoStatus.Information = 0;
			return FLT_PREOP_COMPLETE;
	}


	bool IsProtectedFile(const String<WCHAR>* file_name)
	{
		bool ret = false;
		kFileMutex.Lock();
		for (int i = 0; i < kProtectedFileList->Size(); i++)
		{
			if ((*kProtectedFileList)[i] == (*file_name))
			{
				ret = true;
				break;
			}
		}
		kFileMutex.Unlock();

		kDirMutex.Lock();
		for (int i = 0; i < kProtectedDirList->Size(); i++)
		{
			if ((*kProtectedDirList)[i].IsPrefixOf((*file_name)))
			{
				ret = true;
				break;
			}
		}
		kDirMutex.Unlock();

		return ret;
	}

	void AddFileToProtectedList(const String<WCHAR>* file_name)
	{
		kFileMutex.Lock();
		kProtectedFileList->PushBack(*file_name);
		kFileMutex.Unlock();
		return;
	}

	void RemoveFileFromProtectedList(const String<WCHAR>* file_name)
	{
		kFileMutex.Lock();
		for (int i = 0; i < kProtectedFileList->Size(); i++)
		{
			if ((*kProtectedFileList)[i] == *file_name)
			{
				kProtectedFileList->EraseUnordered(i);
				break;
			}
		}
		kFileMutex.Unlock();
		return;
	}

	bool IsProtectedDir(const String<WCHAR>* dir_name)
	{
		bool ret = false;
		kDirMutex.Lock();
		for (int i = 0; i < kProtectedDirList->Size(); i++)
		{
			if ((*kProtectedDirList)[i] == (*dir_name))
			{
				ret = true;
				break;
			}
		}
		kDirMutex.Unlock();
		return ret;
	}

	void AddDirToProtectedList(const String<WCHAR>* file_name)
	{
		kDirMutex.Lock();
		kProtectedDirList->PushBack(*file_name);
		kDirMutex.Unlock();
		return;
	}

	void RemoveDirFromProtectedList(const String<WCHAR>* dir_name)
	{
		kDirMutex.Lock();
		for (int i = 0; i < kProtectedDirList->Size(); i++)
		{
			if ((*kProtectedDirList)[i] == *dir_name)
			{
				kProtectedDirList->EraseUnordered(i);
				break;
			}
		}
		kDirMutex.Unlock();
		return;
	}

}


