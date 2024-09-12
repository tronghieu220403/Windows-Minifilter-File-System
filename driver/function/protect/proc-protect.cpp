#include "proc-protect.h"

namespace protect_proc
{
	void DrvRegister()
	{
		kProtectProcIdList = new Vector<eprocess::ProcInfo>();
		kProtectProcImageList = new Vector<String<WCHAR>>();
		kProcIdMutex.Create();
		kProcImageMutex.Create();
		NTSTATUS status;

		status = PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&protect_proc::ProcessNotifyCallBack, FALSE);
		if (STATUS_SUCCESS != status)
		{
			DebugMessage("Fail to register: %x", status);
		}
	}
	void DrvUnload()
	{
		PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&protect_proc::ProcessNotifyCallBack, TRUE);
		delete kProtectProcIdList;
		delete kProtectProcImageList;

	}

	void ProcessNotifyCallBack(PEPROCESS eprocess, size_t pid, PPS_CREATE_NOTIFY_INFO create_info)
	{
		if (create_info) // Process creation
		{
			if (create_info->ImageFileName == nullptr || create_info->FileOpenNameAvailable == FALSE)
			{
				return;
			}

			String<WCHAR> process_image_name(*(create_info)->ImageFileName);
			if (String<WCHAR>(L"\\??\\").IsPrefixOf(process_image_name))
			{
				process_image_name = &process_image_name[String<WCHAR>(L"\\??\\").Size()];
			}

			
		}
		else // Process termination
		{

		}
	}
}

