#include "ps-monitor.h"

namespace process
{
	void FltRegister()
	{
	}

	void FltUnload()
	{
	}

	void DrvRegister()
	{
		eprocess::DrvRegister();
		kTrustedProcessList = new Vector<size_t>();
		ppid = new int[300000];
		::ZeroMemory(ppid, 300000);
	}

	void DrvUnload()
	{
		delete kTrustedProcessList;
		delete[] ppid;
	}

	bool IsTrustedProcess(size_t pid)
	{
		if (pid == SYSTEM_PROCESS_ID)
		{
			return true;
		}
		bool ret = false;
		kProcessMutex.Lock();
		for (size_t i = 0; i < kTrustedProcessList->Size(); i++)
		{
			if ((*kTrustedProcessList)[i] == pid)
			{
				ret = true;
				break;
			}
		}
		kProcessMutex.Unlock();
		return ret;
	}
	void AddTrustedProcess(size_t pid)
	{
		kProcessMutex.Lock();
		kTrustedProcessList->PushBack(pid);
		kProcessMutex.Unlock();
		return;
	}

	void RemoveTrustedProcess(size_t pid)
	{
		kProcessMutex.Lock();
		for (size_t i = 0; i < kTrustedProcessList->Size(); i++)
		{
			if ((*kTrustedProcessList)[i] == pid)
			{
				kTrustedProcessList->EraseUnordered(i);
				break;
			}
		}
		kProcessMutex.Unlock();
		return;
	}
}

