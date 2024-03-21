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
		kTrustedProcessList = new Vector<size_t>();
	}

	void DrvUnload()
	{
		delete kTrustedProcessList;
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

