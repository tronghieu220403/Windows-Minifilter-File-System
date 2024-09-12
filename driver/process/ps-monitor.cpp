#include "ps-monitor.h"

namespace process
{

	void DrvRegister()
	{
		eprocess::DrvRegister();
		kProcessMutex.Create();
		kTrustedProcessList = new Vector<size_t>();
		ppid = new int[300000];
		::ZeroMemory(ppid, 300000);
	}

	void DrvUnload()
	{
		delete kTrustedProcessList;
		delete[] ppid;
		kTrustedProcessList = nullptr;
		ppid = nullptr;
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

	String<WCHAR> GetProcessImageName(size_t pid)
	{
		NTSTATUS status;
		ULONG returned_len;
		HANDLE h_process;
		PEPROCESS e_process;
		UNICODE_STRING name;

		status = PsLookupProcessByProcessId((HANDLE)pid, &e_process);
		if (!NT_SUCCESS(status))
		{
			DebugMessage("PsLookupProcessByProcessId Failed: %08x\n", status);

		}
		status = ObOpenObjectByPointer(e_process, 0, NULL, 0, 0, KernelMode, &h_process);
		if (!NT_SUCCESS(status)) {
			DebugMessage("ObOpenObjectByPointer Failed: %08x\n", status);
		}
		ObDereferenceObject(e_process);

		/* Query the actual size of the process path */
		status = ZwQueryInformationProcess(h_process,
			ProcessImageFileName,
			NULL, // buffer
			0, // buffer size
			&returned_len);

		if (STATUS_INFO_LENGTH_MISMATCH != status) {
			return String<WCHAR>();
		}

		Vector<UCHAR> buffer;
		buffer.Resize(returned_len + 520);

		/* Retrieve the process path from the handle to the process */
		status = ZwQueryInformationProcess(h_process,
			ProcessImageFileName,
			buffer.Data(),
			buffer.Size(),
			&returned_len);

		if (!NT_SUCCESS(status))
		{
			return String<WCHAR>();
		}
		
		String<WCHAR> process_image_name((PUNICODE_STRING)buffer.Data());

		return process_image_name;
	}
}

