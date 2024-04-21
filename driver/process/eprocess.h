#pragma once

#include "../../std/string/string.h"
#include "../../std/vector/vector.h"
#include "../../std/sync/mutex.h"

#include "../../template/system_routine.h"

#pragma warning( disable : 6001)

#define SYSTEM_PROCESS_ID 4

namespace eprocess
{
	size_t GetAplRva();
	size_t GetPidRva();

	inline size_t kPidRva = 0;
	inline size_t kAplRva = 0;
	void DrvRegister();

	class ProcInfo
	{
	private:
		PEPROCESS eproc_ = nullptr;
		String<WCHAR> name_;

		size_t pid_ = 0;
		size_t parent_pid_ = 0;

		PLIST_ENTRY active_process_links_ = nullptr;

	public:
		ProcInfo() = default;

		ProcInfo(const ProcInfo& proc) : eproc_(proc.eproc_) {};
		ProcInfo(const PEPROCESS& eproc) : eproc_(eproc) {};

		ProcInfo(const size_t pid);

		ProcInfo& operator=(const ProcInfo& proc);

		PEPROCESS GetPeprocess() const;
		PEPROCESS GetNextProc() const;
		PEPROCESS GetPrevProc() const;
		PLIST_ENTRY GetActiveProcessLinks();

		String<WCHAR> GetName() const;
		size_t GetPid() const;
		size_t GetParentPid() const;

		String<WCHAR> GetProcessImageName() const;

		void SetNextEntryProc(const PEPROCESS& eproc);
		void SetPrevEntryProc(const PEPROCESS& eproc);

		void SetName(const String<WCHAR>& name);
		void SetPid(const size_t pid);
		void SetParentPid(const size_t parent_pid);

		void DetachFromProcessList();
		void JoinToProcessList();
		bool IsDetached();

	};

}
