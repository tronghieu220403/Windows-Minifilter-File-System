#include "file-protect.h"

namespace protect_file
{
	void FltRegister()
	{
		kProtectedFileList = new Vector<String<WCHAR>>();
		kProtectedDirList = new Vector<String<WCHAR>>();
		kFileMutex.Create();
		kDirMutex.Create();
		reg::kFltFuncVector->PushBack({ IRP_MJ_SET_INFORMATION, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_QUERY_INFORMATION, PreOperation, nullptr });
		//reg::kFltFuncVector->PushBack({ IRP_MJ_CLEANUP, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_CREATE, PreOperation, nullptr });
		//reg::kFltFuncVector->PushBack({ IRP_MJ_CLOSE, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_WRITE, PreOperation, nullptr });
		// reg::kFltFuncVector->PushBack({ IRP_MJ_READ, PreOperation, nullptr });
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

		String<WCHAR> name(GetFileFullPathName(data));

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
		// Not directory
		if (!IsDir && IsProtectedFile(&name))
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
		}

		if ((IsDir && IsProtectedDir(&name)) || (!IsDir && IsProtectedFile(&name)))
		{
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

			/* 
			// This field is only for file deletion
			// Process requests with FileDispositionInformation, FileDispositionInformationEx or file renames
			if (data->Iopb->MajorFunction == IRP_MJ_SET_INFORMATION)
			{
				switch (data->Iopb->Parameters.SetFileInformation.FileInformationClass)
				{
				case FileRenameInformation:
				case FileRenameInformationEx:
				case FileDispositionInformation:
				case FileDispositionInformationEx:
				case FileRenameInformationBypassAccessCheck:
				case FileRenameInformationExBypassAccessCheck:
					goto return_access_denided;
				default:
					goto return_success_no_callback;
				}
			}
			*/
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
		kDirMutex.Lock();
		bool ret = false;
		for (int i = 0; i < kProtectedDirList->Size(); i++)
		{
			if (((String<WCHAR>*)dir_name)->IsPrefixOf((*kProtectedDirList)[i]))
			{
				ret = true;
				break;
			}
		}
		if (ret == true)
		{
			return ret;
		}
		kDirMutex.Unlock();
		kFileMutex.Lock();
		for (int i = 0; i < kProtectedFileList->Size(); i++)
		{
			if (((String<WCHAR>*)dir_name)->IsPrefixOf((*kProtectedFileList)[i]))
			{
				ret = true;
				break;
			}
		}
		kFileMutex.Unlock();
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

	String<WCHAR> GetFileFullPathName(PFLT_CALLBACK_DATA data)
	{
		if (data == nullptr)
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

}


