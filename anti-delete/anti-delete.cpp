#include "anti-delete.h"

namespace anti_delete
{
	void FltRegister()
	{
		kFltFuncVector->PushBack({ IRP_MJ_CREATE, PreOperation, nullptr });
		kFltFuncVector->PushBack({ IRP_MJ_SET_INFORMATION, PreOperation, nullptr });
		return;
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

		String<char> file_name = GetFileName(data);

		if (IsProtectedFile(file_name) == false)
		{
			return FLT_PREOP_SUCCESS_NO_CALLBACK;
		}

		// Ignore un

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

	String<char> GetFileName(PFLT_CALLBACK_DATA data)
	{
		return String<char>();
	}

	bool IsProtectedFile(String<char>& file_name)
	{
		return false;
	}

	bool AddFileToProtectedList()
	{
		return false;
	}

}


