#include "hide-proc.h"


namespace hide_proc
{
	void DrvRegister()
	{
		kHideProcIdList = new Vector<eprocess::ProcInfo>();
		kProcIdMutex.Create();
		kProcImageMutex.Create();
		PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&hide_proc::ProcessNotifyCallBack, FALSE);
	}

	void DrvUnload()
	{
		delete kHideProcIdList;
		PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&hide_proc::ProcessNotifyCallBack, TRUE);
	}

	bool HideProc(const eprocess::ProcInfo* info)
	{

		return true;
	}

	bool UnhideProc(const eprocess::ProcInfo* info)
	{
		return true;
	}

	bool IsHiddenProc(size_t pid)
	{
		bool ret = false;
		kProcIdMutex.Lock();
		for (int i = 0; i < kHideProcIdList->Size(); i++)
		{
			if ((*kHideProcIdList)[i].GetParentPid() == pid ||
				(*kHideProcIdList)[i].GetPid() == pid)
			{
				ret = true;
				break;
			}
		}
		kProcIdMutex.Unlock();
		return ret;
	}

	void AddProcIdToHideList(size_t pid)
	{
		kProcIdMutex.Lock();
		kHideProcIdList->PushBack(pid);
		HideProc(eprocess::ProcInfo(pid));
		kProcIdMutex.Unlock();
		return;

	}
	void DeleteProcIdFromHideList(size_t pid)
	{
		kProcIdMutex.Lock();
		for (int i = 0; i < kHideProcIdList->Size(); i++)
		{
			if ((*kHideProcIdList)[i].GetPid() == pid)
			{
				kHideProcIdList->EraseUnordered(i);
				UnhideProc(eprocess::ProcInfo(pid));
				break;
			}
		}
		kProcIdMutex.Unlock();
		return;
	}

	void AddProcImageToHideList(const String<WCHAR>* image_path)
	{

	}

	void DeleteProcImageFromHideList(const String<WCHAR>* image_path)
	{

	}

	void HideOnInitializeOperation()
	{

	}

	void ProcessNotifyCallBack(PEPROCESS eprocess, size_t pid, PPS_CREATE_NOTIFY_INFO create_info)
	{
		
	}


}

