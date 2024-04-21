#include "eprocess.h"

namespace eprocess
{
	ProcInfo::ProcInfo(const PEPROCESS& eproc)
	{
		eproc_ = eproc; 
		pid_ = GetPid();
		active_process_links_ = GetActiveProcessLinks();
	}
	ProcInfo::ProcInfo(const size_t pid)
	{
		if (KeGetCurrentIrql() != PASSIVE_LEVEL && KeGetCurrentIrql() != APC_LEVEL)
		{
			// Debug message to know if this error is happening
			return;
		}
		PsLookupProcessByProcessId((HANDLE)pid, (PEPROCESS*)&eproc_);
		if (eproc_ != NULL)
		{
			ObDereferenceObject(eproc_);
		}
		active_process_links_ = PLIST_ENTRY((PUCHAR)eproc_ + kAplRva);
	}

	ProcInfo& ProcInfo::operator=(const ProcInfo& proc)
	{
		eproc_ = proc.eproc_;
		pid_ = GetPid();
		GetActiveProcessLinks();
		return *this;
	}

	PEPROCESS ProcInfo::GetPeprocess() const
	{
		return eproc_;
	}


	PLIST_ENTRY ProcInfo::GetActiveProcessLinks()
	{
		if (active_process_links_ == nullptr)
		{
			active_process_links_ = (PLIST_ENTRY)((PUCHAR)eproc_ + kAplRva);
		}
		return active_process_links_;
	}

	String<WCHAR> ProcInfo::GetName() const
	{
		return name_;
	}

	size_t ProcInfo::GetPid()
	{
		if (pid_ == 0)
		{
			pid_ = *(size_t*)((PUCHAR)eproc_ + kPidRva);
		}
		return pid_;
	}

	size_t ProcInfo::GetParentPid() const
	{
		return parent_pid_;
	}

	String<WCHAR> ProcInfo::GetProcessImageName() const
	{
		String<WCHAR> process_image_name;
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		ULONG returned_length;
		HANDLE h_process = NULL;

		PAGED_CODE(); // this eliminates the possibility of the IDLE Thread/Process

		if (eproc_ == NULL)
		{
			return String<WCHAR>();
		}

		status = ObOpenObjectByPointer(eproc_,
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


	PEPROCESS ProcInfo::GetNextProc() const
	{
		LIST_ENTRY* apl = active_process_links_->Flink;
		return (PEPROCESS)((PUCHAR)apl - kAplRva);
	}

	PEPROCESS ProcInfo::GetPrevProc() const
	{
		LIST_ENTRY* apl = active_process_links_->Blink;
		return (PEPROCESS)((PUCHAR)apl - kAplRva);
	}

	void ProcInfo::SetNextEntryProc(const PEPROCESS& eproc)
	{
		active_process_links_->Flink = eprocess::ProcInfo(eproc).GetActiveProcessLinks();
		/*
		DebugMessage("SetNextEntryProc");
		DebugMessage("Cur eporcess: %p", eproc_);
		DebugMessage("%p", active_process_links_->Flink);
		DebugMessage("%llx", *(LONG64*)(active_process_links_->Flink));
		DebugMessage("%p", (LONG64*)(active_process_links_->Flink));
		DebugMessage("%p", (LONG64*)&(active_process_links_->Flink));
		DebugMessage("%p", (LONG64*)(ProcInfo(eproc).GetActiveProcessLinks()));
		DebugMessage("");
		*/
		//InterlockedExchange64((LONG64 *)&(active_process_links_->Flink), (LONG64)ProcInfo(eproc).GetActiveProcessLinks());
	}

	void ProcInfo::SetPrevEntryProc(const PEPROCESS& eproc)
	{
		active_process_links_->Blink = eprocess::ProcInfo(eproc).GetActiveProcessLinks();
		/*
		DebugMessage("SetPrevEntryProc");
		DebugMessage("Cur eporcess: %p", eproc_);
		DebugMessage("%p", active_process_links_->Blink);
		DebugMessage("%llx", *(LONG64*)(active_process_links_->Blink));
		DebugMessage("%p", (LONG64*)(active_process_links_->Blink));
		DebugMessage("%p", (LONG64*)&(active_process_links_->Blink));
		DebugMessage("%p", (LONG64*)(ProcInfo(eproc).GetActiveProcessLinks()));
		DebugMessage("");
		*/
		//InterlockedExchange64((LONG64 *)&(active_process_links_->Blink), (LONG64)ProcInfo(eproc).GetActiveProcessLinks());
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

	void ProcInfo::DetachFromProcessList()
	{
		kEprocssMutex.Lock();
		__try
		{
			//RemoveEntryList(GetActiveProcessLinks());
			//ProcInfo(GetPrevProc()).SetNextEntryProc(GetNextProc());
			//ProcInfo(GetNextProc()).SetPrevEntryProc(GetPrevProc());
			//SetPrevEntryProc(eproc_);
			//SetNextEntryProc(eproc_);
			//GetActiveProcessLinks()->Blink = GetActiveProcessLinks();
			//GetActiveProcessLinks()->Flink = GetActiveProcessLinks();
			PLIST_ENTRY prev, next;
			PLIST_ENTRY cur = GetActiveProcessLinks();
			prev = cur->Blink;
			next = cur->Flink;
			prev->Flink = next;
			next->Blink = prev;
			//DebugMessage("% p", prev);
			//DebugMessage("% p", prev->);

			//cur->Flink = (PLIST_ENTRY)&cur->Flink;
			//cur->Blink = (PLIST_ENTRY)&cur->Flink;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			DebugMessage("Catch a bug");
		}
		kEprocssMutex.Unlock();
	}

	void ProcInfo::JoinToProcessList()
	{
		// What if SYSTEM_PROCESS_ID is detached? We will have to use ZwQueryObject or PsLookupProcessByProcessId to find a process that is not detached
		ProcInfo system_proc(SYSTEM_PROCESS_ID);

		ProcInfo next_proc(system_proc.GetNextProc());
		system_proc.SetNextEntryProc(eproc_);
		SetNextEntryProc(next_proc.GetPeprocess());
		SetPrevEntryProc(system_proc.GetPeprocess());
		next_proc.SetPrevEntryProc(eproc_);
	}

	bool ProcInfo::IsDetached()
	{
		if (GetNextProc() == GetPeprocess() && GetPrevProc() == GetPeprocess())
		{
			return true;
		}
		return false;
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
		kEprocssMutex.Create();
	}
}

