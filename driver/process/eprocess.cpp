#include "eprocess.h"

namespace eprocess
{
	ProcInfo::ProcInfo(const size_t pid)
	{
		if (KeGetCurrentIrql() != PASSIVE_LEVEL && KeGetCurrentIrql() != APC_LEVEL)
		{
			// Debug message to know if this error is happening
			return;
		}
		PsLookupProcessByProcessId((HANDLE)pid, (PEPROCESS*)&eproc_);
	}

	ProcInfo& ProcInfo::operator=(const ProcInfo& proc)
	{
		eproc_ = proc.eproc_;
		pid_ = (size_t)PsGetProcessId(eproc_);
		return *this;
	}

	PEPROCESS ProcInfo::GetPrevProc() const
	{
		return PEPROCESS();
	}

	PEPROCESS ProcInfo::GetNextProc() const
	{
		return PEPROCESS();
	}

	String<WCHAR> ProcInfo::GetName() const
	{
		return name_;
	}

	size_t ProcInfo::GetPid() const
	{
		return pid_;
	}

	size_t ProcInfo::GetParentPid() const
	{
		return parent_pid_;
	}

	void ProcInfo::SetNextProc(const PEPROCESS& eproc)
	{
	}

	void ProcInfo::SetPrevProc(const PEPROCESS& eproc)
	{
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
			// Confuse, can only be used in IRQL <= APC_LEVEL
			if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)i, &eprocs[idx])))
			{
				pids[idx] = i;
				idx++;
			}
		}

		// Go through the EPROCESS structure and look for the PID
		// we can start at 0x20 because UniqueProcessId should
		// not be in the first 0x20 bytes,
		// also we should stop after 0x300 bytes with no success
		for (int i = 0x20; i < 0x300; i += 4)
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

