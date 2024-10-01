#include "ps-monitor.h"

namespace process
{

	void DrvRegister()
	{
		eprocess::DrvRegister();
		kTrustedProcessMutex.Create();
		kTrustedProcessList = new Vector<String<WCHAR>>();
	}

	void DrvUnload()
	{
		delete kTrustedProcessList;
		kTrustedProcessList = nullptr;
	}

	bool IsTrustedProcess(const String<WCHAR>& name)
	{
		bool ret = false;
		kTrustedProcessMutex.Lock();
		for (size_t i = 0; i < kTrustedProcessList->Size(); i++)
		{
			if ((*kTrustedProcessList)[i] == name)
			{
				ret = true;
				break;
			}
		}
		kTrustedProcessMutex.Unlock();
		return ret;
	}

	void AddTrustedProcess(const String<WCHAR>& name)
	{
		kTrustedProcessMutex.Lock();
		kTrustedProcessList->PushBack(name);
		kTrustedProcessMutex.Unlock();
		return;
	}

	void RemoveTrustedProcess(const String<WCHAR>& name)
	{
		kTrustedProcessMutex.Lock();
		for (size_t i = 0; i < kTrustedProcessList->Size(); i++)
		{
			if ((*kTrustedProcessList)[i] == name)
			{
				kTrustedProcessList->EraseUnordered(i);
				break;
			}
		}
		kTrustedProcessMutex.Unlock();
		return;
	}

	String<WCHAR> GetProcessImageName(size_t pid)
	{
		NTSTATUS status;
		ULONG returned_len;
		HANDLE h_process = nullptr;
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
			ZwClose(h_process);
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
			ZwClose(h_process);
			return String<WCHAR>();
		}
		
		String<WCHAR> process_image_name((PUNICODE_STRING)buffer.Data());

		ZwClose(h_process);
		return process_image_name;
	}
}

