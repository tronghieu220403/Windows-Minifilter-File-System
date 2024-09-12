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

		delete kProtectProcIdList;
		delete kProtectProcImageList;

	}

	void ProcessNotifyCallBack(PEPROCESS eprocess, size_t pid, PPS_CREATE_NOTIFY_INFO create_info)
	{
		if (kEnableProtectProc == false)
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
		if (kEnableProtectProc == false)
		{
			return OB_PREOP_CALLBACK_STATUS();
		}
		return OB_PREOP_CALLBACK_STATUS();
	}
}

