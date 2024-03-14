#include "anti-delete.h"

namespace anti_delete
{
	void FltRegister()
	{
		kFltFuncVector->PushBack({ IRP_MJ_CREATE, PreOperation, nullptr });
		kFltFuncVector->PushBack({ IRP_MJ_SET_INFORMATION, PreOperation, nullptr });
		return;
	}

	FLT_PREOP_CALLBACK_STATUS PreOperation(_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, _Flt_CompletionContext_Outptr_ PVOID* CompletionContext)
	{
		UNREFERENCED_PARAMETER(CompletionContext);

		PAGED_CODE();

		FLT_PREOP_CALLBACK_STATUS ret = FLT_PREOP_SUCCESS_NO_CALLBACK;

		// Ignore directories
		BOOLEAN IsDir;
		NTSTATUS status = FltIsDirectory(FltObjects->FileObject, FltObjects->Instance, &IsDir);
		if (NT_SUCCESS(status)) {
			if (IsDir) {
				return FLT_PREOP_SUCCESS_NO_CALLBACK;
			}
		}

		if (IsProtectedFile() == false)
		{
			return FLT_PREOP_SUCCESS_NO_CALLBACK;
		}

		// Ignore un

		// https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-create
		// When the system tries to open a handle to a file object,
		// detect requests that have DELETE_ON_CLOSE in DesiredAccess
		if (Data->Iopb->MajorFunction == IRP_MJ_CREATE) {
			if (!FlagOn(Data->Iopb->Parameters.Create.Options, FILE_DELETE_ON_CLOSE)) {
				return FLT_PREOP_SUCCESS_NO_CALLBACK;
			}
		}

		// Process requests with FileDispositionInformation, FileDispositionInformationEx  or file renames
		if (Data->Iopb->MajorFunction == IRP_MJ_SET_INFORMATION) {
			switch (Data->Iopb->Parameters.SetFileInformation.FileInformationClass) {
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

	String<char> GetFileName(PFLT_CALLBACK_DATA Data)
	{
		return String<char>();
	}

	bool IsProtectedFile(String<char>& s)
	{
		return false;
	}

	bool AddFileToProtectedList()
	{
		return false;
	}

}

