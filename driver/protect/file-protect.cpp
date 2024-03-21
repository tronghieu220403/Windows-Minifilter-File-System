#include "file-protect.h"

namespace protection
{
	void FltRegister()
	{
		kProtectedFileList = new Vector<String<WCHAR>>();
		kProtectedDirList = new Vector<String<WCHAR>>();
		kFileMutex.Create();
		kDirMutex.Create();
		reg::kFltFuncVector->PushBack({ IRP_MJ_CREATE, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_SET_INFORMATION, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_QUERY_INFORMATION, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_CLEANUP, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_CLOSE, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_CREATE, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_WRITE, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_READ, PreOperation, nullptr });
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

		if (flt::IsTrustedRequestor(data, (size_t)PsGetCurrentProcessId()) == true)
		{
			return FLT_PREOP_SUCCESS_NO_CALLBACK;
		}

		String<WCHAR> name(GetFileFullPathName(data));

		BOOLEAN IsDir;
		if (!NT_SUCCESS(FltIsDirectory(flt_objects->FileObject, flt_objects->Instance, &IsDir)))
		{
			// Not directory
			if (!IsDir) 
			{
				if (IsProtectedFile(name)) {
					goto return_access_denided;
				}
				goto return_success;
			}
		}

		if (IsProtectedDir(name) == true)
		{
			// https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-create
			// When the system tries to open a handle to a file object,
			// detect requests that have DELETE_ON_CLOSE in DesiredAccess
			if (data->Iopb->MajorFunction == IRP_MJ_CREATE) 
			{
				if (!FlagOn(data->Iopb->Parameters.Create.Options, FILE_DELETE_ON_CLOSE)) 
				{
					goto return_success;
				}
			}

			// Process requests with FileDispositionInformation, FileDispositionInformationEx  or file renames
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
					goto return_success;
				}
			}
		}

	return_success:
		return FLT_PREOP_SUCCESS_NO_CALLBACK;

	return_access_denided:
		data->IoStatus.Status = STATUS_ACCESS_DENIED;
		data->IoStatus.Information = 0;
		return FLT_PREOP_COMPLETE;

	}


	bool IsProtectedFile(String<WCHAR>& file_name)
	{
		bool ret = false;
		kFileMutex.Lock();
		for (int i = 0; i < kProtectedFileList->Size(); i++)
		{
			if ((*kProtectedFileList)[i] == file_name)
			{
				ret = true;
				break;
			}
		}
		kFileMutex.Unlock();
		return ret;
	}

	void AddFileToProtectedList(String<WCHAR>& file_name)
	{
		kFileMutex.Lock();
		kProtectedFileList->PushBack(file_name);
		kFileMutex.Unlock();
		return;
	}

	void RemoveFileFromProtectedList(String<WCHAR>& file_name)
	{
		kFileMutex.Lock();
		for (int i = 0; i < kProtectedFileList->Size(); i++)
		{
			if ((*kProtectedFileList)[i] == file_name)
			{
				kProtectedFileList->EraseUnordered(i);
				break;
			}
		}
		kFileMutex.Unlock();
		return;
	}

	bool IsProtectedDir(String<WCHAR>& dir_name)
	{
		kDirMutex.Lock();
		bool ret = false;
		for (int i = 0; i < kProtectedDirList->Size(); i++)
		{
			if (dir_name.IsPrefixOf((*kProtectedDirList)[i]))
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
			if (dir_name.IsPrefixOf((*kProtectedFileList)[i]))
			{
				ret = true;
				break;
			}
		}
		kFileMutex.Unlock();
		return ret;
	}

	void AddDirToProtectedList(String<WCHAR>& file_name)
	{
		kDirMutex.Lock();
		kProtectedDirList->PushBack(file_name);
		kDirMutex.Unlock();
		return;
	}

	void RemoveDirFromProtectedList(String<WCHAR>& dir_name)
	{
		kDirMutex.Lock();
		for (int i = 0; i < kProtectedDirList->Size(); i++)
		{
			if ((*kProtectedDirList)[i] == dir_name)
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
		FltGetFileNameInformation(data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP, &file_name_info);
		res = String<WCHAR>(file_name_info->Name);
		FltReleaseFileNameInformation(file_name_info);
		
		return res;
	}

}


