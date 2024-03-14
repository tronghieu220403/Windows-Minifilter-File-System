#include "anti-delete.h"

namespace anti_delete
{
	void FltRegister()
	{
		kAntiList = new Vector<String<WCHAR>>();
		kMutex.Create();
		reg::kFltFuncVector->PushBack({ IRP_MJ_CREATE, PreOperation, nullptr });
		reg::kFltFuncVector->PushBack({ IRP_MJ_SET_INFORMATION, PreOperation, nullptr });
		return;
	}

	void FltUnload()
	{
		delete kAntiList;
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

		FLT_PREOP_CALLBACK_STATUS ret = FLT_PREOP_SUCCESS_NO_CALLBACK;

		// Ignore directories
		BOOLEAN IsDir;
		NTSTATUS status = FltIsDirectory(flt_objects->FileObject, flt_objects->Instance, &IsDir);
		if (NT_SUCCESS(status)) {
			if (IsDir) {
				return FLT_PREOP_SUCCESS_NO_CALLBACK;
			}
		}

		String<WCHAR> file_name = GetFileFullPathName(data);

		if (IsProtectedFile(file_name) == false)
		{
			return FLT_PREOP_SUCCESS_NO_CALLBACK;
		}

		// https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-create
		// When the system tries to open a handle to a file object,
		// detect requests that have DELETE_ON_CLOSE in DesiredAccess
		if (data->Iopb->MajorFunction == IRP_MJ_CREATE) {
			if (!FlagOn(data->Iopb->Parameters.Create.Options, FILE_DELETE_ON_CLOSE)) {
				return FLT_PREOP_SUCCESS_NO_CALLBACK;
			}
		}

		// Process requests with FileDispositionInformation, FileDispositionInformationEx  or file renames
		if (data->Iopb->MajorFunction == IRP_MJ_SET_INFORMATION) {
			switch (data->Iopb->Parameters.SetFileInformation.FileInformationClass) {
			case FileRenameInformation:
			case FileRenameInformationEx:
			case FileDispositionInformation:
			case FileDispositionInformationEx:
			case FileRenameInformationBypassAccessCheck:
			case FileRenameInformationExBypassAccessCheck:
				break;

			default:
				return FLT_PREOP_SUCCESS_NO_CALLBACK;
			}
		}

		return FLT_PREOP_SUCCESS_NO_CALLBACK;

	}

	String<WCHAR> GetFileFullPathName(PFLT_CALLBACK_DATA data)
	{
		if (data == nullptr)
		{
			return String<WCHAR>();
		}

		String<WCHAR> res;

		if (data->Iopb != NULL && data->Iopb->TargetFileObject != NULL)
		{
			UNICODE_STRING uni_str;
			uni_str.Buffer = data->Iopb->TargetFileObject->FileName.Buffer;
			uni_str.MaximumLength = data->Iopb->TargetFileObject->FileName.MaximumLength;
			uni_str.Length = data->Iopb->TargetFileObject->FileName.Length;

			res = String<WCHAR>(uni_str);

		}
		else
		{
			PFLT_FILE_NAME_INFORMATION file_name_info;
			FltGetFileNameInformation(data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP, &file_name_info);

			res = String<WCHAR>(file_name_info->Name);

			FltReleaseFileNameInformation(file_name_info);
		}
		
		return res;
	}

	bool IsProtectedFile(String<WCHAR>& file_name)
	{
		bool ret = false;
		kMutex.Lock();
		for (int i = 0; i < kAntiList->Size(); i++)
		{
			if ((*kAntiList)[i] == file_name)
			{
				ret = true;
				break;
			}
		}
		kMutex.Unlock();
		return ret;
	}

	void AddFileToProtectedList(String<WCHAR>& s)
	{
		kMutex.Lock();
		kAntiList->PushBack(s);
		kMutex.Unlock();
		return;
	}

}


