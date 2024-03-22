#include "hide-proc.h"

namespace hide
{
	void DrvRegister()
	{
		PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&hide::ProcessNotifyCallBack, FALSE);
	}

	void DrvUnload()
	{
		PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&hide::ProcessNotifyCallBack, TRUE);
	}

	ProcInfo::ProcInfo(const size_t pid)
	{
		PsLookupProcessByProcessId((HANDLE)pid, (PEPROCESS*)&eproc_);
	}

	ProcInfo& ProcInfo::operator=(const ProcInfo& proc)
	{
		eproc_ = proc.eproc_;
		return *this;
	}

	PEPROCESS ProcInfo::GetNextProc() const
	{

	}

	PEPROCESS ProcInfo::GetPrevProc() const
	{

	}

	bool HideProc(const ProcInfo& info)
	{
		return true;
	}

	bool IsHiddenProc(size_t pid)
	{

	}

	void AddProcToHideList(size_t pid)
	{

	}
	void DeleteProcFromHideList(size_t pid)
	{

	}

	void ProcessNotifyCallBack(PEPROCESS Process, HANDLE ProcessId, PPS_CREATE_NOTIFY_INFO CreateInfo)
	{

	}


}

