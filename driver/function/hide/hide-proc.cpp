#include "hide-proc.h"

namespace hide
{
	void DrvRegister()
	{
		kHideProcList = new Vector<ProcInfo>();
		kProcMutex.Create();
		PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&hide::ProcessNotifyCallBack, FALSE);
	}

	void DrvUnload()
	{
		delete kHideProcList;
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

	String<WCHAR> ProcInfo::GetName() const
	{
		return name_;
	}

	size_t ProcInfo::GetPid() const
	{
		return pid_;
	}

	size_t ProcInfo::GetParentPid() const
	{
		return parent_pid_;
	}

	size_t ProcInfo::GetAncestorPid() const
	{
		return ancestor_pid_;
	}

	void ProcInfo::SetName(const String<WCHAR>& name)
	{
		name_ = name;
	}

	void ProcInfo::SetPid(const size_t pid)
	{
		pid_ = pid;
	}

	void ProcInfo::SetParentPid(const size_t parent_pid)
	{
		parent_pid_ = parent_pid;
	}

	void ProcInfo::SetAncestorPid(const size_t ancestor_pid)
	{
		ancestor_pid_ = ancestor_pid;
	}

	bool HideProc(const ProcInfo& info)
	{
		return true;
	}

	bool IsHiddenProc(size_t pid)
	{
		bool ret = false;
		kProcMutex.Lock();
		for (int i = 0; i < kHideProcList->Size(); i++)
		{
			if ((*kHideProcList)[i].GetAncestorPid() == pid ||
				(*kHideProcList)[i].GetParentPid() == pid ||
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
				break;
			}
		}
		kProcMutex.Unlock();
		return;
	}

	void ProcessNotifyCallBack(PEPROCESS eprocess, size_t pid, PPS_CREATE_NOTIFY_INFO create_info)
	{
		// create
		if (create_info != NULL)
		{
			// need to check for the case that a process does not have PPID
			if (process::kProcAncestor.Find((size_t)create_info->ParentProcessId) == process::kProcAncestor.End())
			{
				process::kProcAncestor.Insert(pid, (size_t)create_info->ParentProcessId);
			}
			else
			{
				size_t apid = process::kProcAncestor[(size_t)create_info->ParentProcessId];
				if (apid == 0)
				{
					process::kProcAncestor.Insert(pid, (size_t)create_info->ParentProcessId);
				}
				else
				{
					process::kProcAncestor.Insert(pid, apid);
				}
			}
		}
		else // destroy
		{
			process::kProcAncestor.Erase(pid);
		}
		return;
	}


}

