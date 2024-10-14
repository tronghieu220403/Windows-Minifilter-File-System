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
		String<WCHAR> process_image_name;
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		PEPROCESS eproc;
		status = PsLookupProcessByProcessId((HANDLE)pid, &eproc);
		if (!NT_SUCCESS(status))
		{
			return String<WCHAR>();
		}

		PUNICODE_STRING process_name = NULL;
		status = SeLocateProcessImageName(eproc, &process_name);
		if (status == STATUS_SUCCESS)
		{
			process_image_name = process_name;
			delete process_name;
			ObDereferenceObject(eproc);
			return process_image_name;
		}

		ObDereferenceObject(eproc);

		ULONG returned_length = 0;
		HANDLE h_process = NULL;
		Vector<UCHAR> buffer;

		if (ZwQueryInformationProcess == NULL)
		{
			DebugMessage("Cannot resolve ZwQueryInformationProcess\n");
			status = STATUS_UNSUCCESSFUL;
			goto cleanUp;
		}

		status = ObOpenObjectByPointer(eproc,
			0, NULL, 0, 0, KernelMode, &h_process);
		if (!NT_SUCCESS(status))
		{
			DebugMessage("ObOpenObjectByPointer Failed: %08x\n", status);
			return String<WCHAR>();
		}

		if (ZwQueryInformationProcess == NULL)
		{
			DebugMessage("Cannot resolve ZwQueryInformationProcess\n");
			status = STATUS_UNSUCCESSFUL;
			goto cleanUp;
		}

		// Query the actual size of the process path 
		status = ZwQueryInformationProcess(h_process,
			ProcessImageFileName,
			NULL, // buffer
			0,    // buffer size
			&returned_length);

		if (STATUS_INFO_LENGTH_MISMATCH != status) {
			DebugMessage("ZwQueryInformationProcess status = %x\n", status);
			goto cleanUp;
		}

		buffer.Resize(returned_length);

		if (buffer.Data() == NULL)
		{
			goto cleanUp;
		}

		// Retrieve the process path from the handle to the process
		status = ZwQueryInformationProcess(h_process,
			ProcessImageFileName,
			buffer.Data(),
			returned_length,
			&returned_length);
		process_image_name = (PUNICODE_STRING)buffer.Data();

	cleanUp:
		ZwClose(h_process);

		return process_image_name;
	}
}

