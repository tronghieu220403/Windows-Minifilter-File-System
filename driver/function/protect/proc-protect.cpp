#include "proc-protect.h"

namespace protect_proc
{
	void DrvRegister()
	{
		kProtectProcImageList = new Vector<String<WCHAR>>();
		kProcImageMutex.Create();
		NTSTATUS status;

		status = PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&protect_proc::ProcessNotifyCallBack, FALSE);
		if (STATUS_SUCCESS != status)
		{
			DebugMessage("Fail to register: %x", status);
		}

		OB_CALLBACK_REGISTRATION obRegistration = { 0, };
		OB_OPERATION_REGISTRATION opRegistration[2] = {};

		obRegistration.Version = ObGetFilterVersion();	// Get version
		obRegistration.OperationRegistrationCount = 2;
		RtlInitUnicodeString(&obRegistration.Altitude, L"2204");
		obRegistration.RegistrationContext = NULL;

		opRegistration[0].ObjectType = PsProcessType;
		opRegistration[0].Operations = OB_OPERATION_HANDLE_CREATE;
		opRegistration[0].PreOperation = PreObCallback;
		opRegistration[0].PostOperation = nullptr;

		opRegistration[1].ObjectType = PsThreadType;
		opRegistration[1].Operations = OB_OPERATION_HANDLE_CREATE;
		opRegistration[1].PreOperation = PreObCallback;
		opRegistration[1].PostOperation = nullptr;

		obRegistration.OperationRegistration = opRegistration;

		status = ObRegisterCallbacks(&obRegistration, &kHandleRegistration);
		if (STATUS_SUCCESS != status)
		{
			DebugMessage("Fail to register ObRegisterCallbacks: %x", status);
		}

	}
	void DrvUnload()
	{
		PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&protect_proc::ProcessNotifyCallBack, TRUE);
		ObUnRegisterCallbacks(kHandleRegistration);

		delete kProtectProcImageList;

	}

	void AddImageToProtectList(const String<WCHAR>& image_name)
	{
		kProcImageMutex.Lock();
		kProtectProcImageList->PushBack(image_name);
		kProcImageMutex.Unlock();
	}

	void DeleteImageFromProtectList(const String<WCHAR>& image_name)
	{
		kProcImageMutex.Lock();
		for (size_t i = 0; i < kProtectProcImageList->Size(); i++)
		{
			if (kProtectProcImageList->At(i) == image_name)
			{
				kProtectProcImageList->EraseUnordered(i);
				break;
			}
		}
		kProcImageMutex.Unlock();
	}

	void ProcessNotifyCallBack(PEPROCESS eprocess, size_t pid, PPS_CREATE_NOTIFY_INFO create_info)
	{
		if (kEnableProcProtect == false)
		{
			return;
		}

		if (create_info) // Process creation
		{
			String<WCHAR> process_image_name(process::GetProcessImageName(pid));
		}
		else // Process termination
		{

		}
	}

	OB_PREOP_CALLBACK_STATUS PreObCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION pOperationInformation)
	{
		if (kEnableProcProtect == false)
		{
			return OB_PREOP_CALLBACK_STATUS();
		}
		return OB_PREOP_CALLBACK_STATUS();
	}
}

