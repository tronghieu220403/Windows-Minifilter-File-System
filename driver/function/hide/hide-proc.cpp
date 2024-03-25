#include "hide-proc.h"


namespace hide_proc
{
	void DrvRegister()
	{
		kHideProcList = new Vector<eprocess::ProcInfo>();
		kProcMutex.Create();
		PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&hide_proc::ProcessNotifyCallBack, FALSE);
	}

	void DrvUnload()
	{
		delete kHideProcList;
		PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&hide_proc::ProcessNotifyCallBack, TRUE);
	}

	bool HideProc(const eprocess::ProcInfo& info)
	{
		return true;
	}

	bool UnhideProc(const eprocess::ProcInfo& info)
	{
		return true;
	}

	bool IsHiddenProc(size_t pid)
	{
		bool ret = false;
		kProcMutex.Lock();
		for (int i = 0; i < kHideProcList->Size(); i++)
		{
			if ((*kHideProcList)[i].GetParentPid() == pid ||
				(*kHideProcList)[i].GetPid() == pid)
			{
				ret = true;
				break;
			}
		}
		kProcMutex.Unlock();
		return ret;
	}

	void AddProcToHideList(size_t pid)
	{
		kProcMutex.Lock();
		kHideProcList->PushBack(pid);
		HideProc(eprocess::ProcInfo(pid));
		kProcMutex.Unlock();
		return;

	}
	void DeleteProcFromHideList(size_t pid)
	{
		kProcMutex.Lock();
		for (int i = 0; i < kHideProcList->Size(); i++)
		{
			if ((*kHideProcList)[i].GetPid() == pid)
			{
				kHideProcList->EraseUnordered(i);
				UnhideProc(eprocess::ProcInfo(pid));
				break;
			}
		}
		kProcMutex.Unlock();
		return;
	}

	void ProcessNotifyCallBack(PEPROCESS eprocess, size_t pid, PPS_CREATE_NOTIFY_INFO create_info)
	{
		
	}


}

