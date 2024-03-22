#pragma once

#include "std/string/string.h"
#include "std/vector/vector.h"
#include "std/sync/mutex.h"

#include "process/ps-monitor.h"

#include "template/register.h"
#include "template/flt-ex.h"

namespace hide
{
	class ProcInfo
	{
	private:
		PEPROCESS eproc_ = nullptr;
		String<WCHAR> name_;
		size_t pid_ = 0;
		size_t parent_pid_ = 0;
		size_t ancestor_pid_ = 0;
	public:
		ProcInfo() = default;

		ProcInfo(const ProcInfo& proc) : eproc_(proc.eproc_) {};
		ProcInfo(const PEPROCESS& eproc) : eproc_(eproc) {};
		
		ProcInfo(const size_t pid);

		ProcInfo& operator=(const ProcInfo& proc);

		PEPROCESS GetNextProc() const;
		PEPROCESS GetPrevProc() const;

		String<WCHAR> GetName() const;
		size_t GetPid() const;
		size_t GetParentPid() const;
		size_t GetAncestorPid() const;

		void SetNextProc(const PEPROCESS& eproc);
		void SetPrevProc(const PEPROCESS& eproc);

		void SetName(const String<WCHAR>& name);
		void SetPid(const size_t pid);
		void SetParentPid(const size_t parent_pid);
		void SetAncestorPid(const size_t ancestor_pid);
	};

	extern inline Vector<ProcInfo>* kHideProcList = nullptr;

	extern inline Mutex kProcMutex;
	
	void FltRegister();

	void FltUnload();

	void DrvRegister();

	void DrvUnload();

	bool HideProc(const ProcInfo& info);

	bool IsHiddenProc(size_t pid);
	void AddProcToHideList(size_t pid);
	void DeleteProcFromHideList(size_t pid);

	void ProcessNotifyCallBack(PEPROCESS, size_t, PPS_CREATE_NOTIFY_INFO);


}