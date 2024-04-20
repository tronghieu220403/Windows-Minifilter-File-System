#include "file-protect.h"

namespace protect_file
{
	void FltRegister()
	{
		kProtectedFileList = new Vector<String<WCHAR>>();
		kProtectedDirList = new Vector<String<WCHAR>>();
		kFileMutex.Create();
		kDirMutex.Create();
		reg::kFltFuncVector->PushBack({ IRP_MJ_DIRECTORY_CONTROL, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_SET_INFORMATION, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_CREATE, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_WRITE, PreOperation, nullptr });
		return;
	}

	void FltUnload()
	{
		delete kProtectedFileList;
		delete kProtectedDirList;
	}

	void DrvRegister()
	{

	}

	void DrvUnload()
	{

	}

	FLT_PREOP_CALLBACK_STATUS PreOperation(_Inout_ PFLT_CALLBACK_DATA data, _In_ PCFLT_RELATED_OBJECTS flt_objects, _Flt_CompletionContext_Outptr_ PVOID* completion_context)
	{
		UNREFERENCED_PARAMETER(completion_context);

		PAGED_CODE();

		if (flt::IsTrustedRequestor(data) == true)
		{
			return FLT_PREOP_SUCCESS_NO_CALLBACK;
		}

		String<WCHAR> name(flt::GetFileFullPathName(data));

		if (name.Size() == 0)
		{
			goto return_success_no_callback;
		}

		BOOLEAN IsDir;
		if (!NT_SUCCESS(FltIsDirectory(flt_objects->FileObject, flt_objects->Instance, &IsDir)))
		{
			goto return_success_no_callback;
		}

		ACCESS_MASK flag;
		PVOID bufffer;

		if ((IsDir && IsProtectedDir(&name)) || (!IsDir && IsProtectedFile(&name)))
		{
			DebugMessage(" ");
			DebugMessage("%s", flt::DebugIrpFlags(data->Iopb->IrpFlags).Data());
			flt::DebugIopbMajorFunc(data->Iopb->MajorFunction);

			switch (data->Iopb->MajorFunction)
			{
			case IRP_MJ_WRITE:
				goto return_access_denided;
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

		// ?????
		if (IsDir && IsProtectedDir(&name))
		{
			switch (data->Iopb->MajorFunction)
			{
			case IRP_MJ_DIRECTORY_CONTROL:
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/irp-mj-directory-control
				switch (data->Iopb->MinorFunction)
				{
				case IRP_MN_NOTIFY_CHANGE_DIRECTORY:
				case IRP_MN_NOTIFY_CHANGE_DIRECTORY_EX:
					goto return_access_denided;
				default:
					break;
				}
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


