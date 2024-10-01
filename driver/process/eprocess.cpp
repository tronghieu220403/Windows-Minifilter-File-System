#include "eprocess.h"

namespace eprocess
{

	String<WCHAR> GetProcessImageName(int pid)
	{
		String<WCHAR> process_image_name;
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		ULONG returned_length;
		HANDLE h_process = NULL;

		PEPROCESS eproc;
		status = PsLookupProcessByProcessId((HANDLE)pid, &eproc);

		if (!NT_SUCCESS(status))
		{
			return String<WCHAR>();
		}

		status = ObOpenObjectByPointer(eproc,
			0, NULL, 0, 0, KernelMode, &h_process);
		if (!NT_SUCCESS(status))
		{
			DebugMessage("ObOpenObjectByPointer Failed: %08x\n", status);
			return String<WCHAR>();
		}

		ObDereferenceObject(eproc);

		if (ZwQueryInformationProcess == NULL)
		{
			DebugMessage("Cannot resolve ZwQueryInformationProcess\n");
			status = STATUS_UNSUCCESSFUL;
			goto cleanUp;
		}

		/* Query the actual size of the process path */
		status = ZwQueryInformationProcess(h_process,
			ProcessImageFileName,
			NULL, // buffer
			0,    // buffer size
			&returned_length);

		if (STATUS_INFO_LENGTH_MISMATCH != status) {
			DebugMessage("ZwQueryInformationProcess status = %x\n", status);
			goto cleanUp;
		}

		process_image_name.Resize(returned_length);

		if (process_image_name.Data() == NULL)
		{
			goto cleanUp;
		}

		/* Retrieve the process path from the handle to the process */
		status = ZwQueryInformationProcess(h_process,
			ProcessImageFileName,
			(PVOID)process_image_name.Data(),
			returned_length,
			&returned_length);


	cleanUp:

		ZwClose(h_process);

		return process_image_name;
	}

	size_t GetAplRva()
	{
		if (KeGetCurrentIrql() != PASSIVE_LEVEL && KeGetCurrentIrql() != APC_LEVEL)
		{
			// Debug message to know if this is happening
			return 0;
		}

		return GetPidRva() + sizeof(INT_PTR);
	}

	size_t GetPidRva()
	{
		DebugMessage("Finding pid rva.");
		if (KeGetCurrentIrql() != PASSIVE_LEVEL && KeGetCurrentIrql() != APC_LEVEL)
		{
			// Debug message to know if this is happening
			return 0;
		}

		ULONG pid_rva = 0; // The offset we're looking for
		int idx = 0;                // Index 
		ULONG pids[3] = { 0 };				// List of PIDs for our 3 processes
		PEPROCESS eprocs[3] = { 0 };		// Process list, will contain 3 processes

		//Select 3 process PIDs and get their EPROCESS Pointer
		for (int i = 16; idx < 3; i += 4)
		{
			if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)i, &eprocs[idx])))
			{
				pids[idx] = i;
				idx++;
			}
		}

		// Go through the EPROCESS structure and look for the PID
		// we can start at 0x20 because Uniqueproc_Id should
		// not be in the first 0x20 bytes,
		// also we should stop after 0x300 bytes with no success
		for (int i = 0x20; i < 0x1000; i += 4)
		{
			if ((*(ULONG*)((UCHAR*)eprocs[0] + i) == pids[0])
				&& (*(ULONG*)((UCHAR*)eprocs[1] + i) == pids[1])
				&& (*(ULONG*)((UCHAR*)eprocs[2] + i) == pids[2]))
			{
				pid_rva = i;
				break;
			}
		}

		ObDereferenceObject(eprocs[0]);
		ObDereferenceObject(eprocs[1]);
		ObDereferenceObject(eprocs[2]);

		return pid_rva;
	}

	void DrvRegister()
	{
		kPidRva = GetPidRva();
		kAplRva = GetAplRva();
	}
}

