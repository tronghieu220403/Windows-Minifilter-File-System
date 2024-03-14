#pragma once

#include "../std/vector/vector.h"
#include "../std/string/string.h"

#include "../template/register.h"

#include <wdm.h>
#include <fltKernel.h>


namespace anti_delete
{
	inline Vector<String<char>>* kAntiList;

	void FltRegister();

	FLT_PREOP_CALLBACK_STATUS PreOperation(_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, _Flt_CompletionContext_Outptr_ PVOID* CompletionContext);

	String<char> GetFileName(PFLT_CALLBACK_DATA Data);

	bool IsProtectedFile(String<char>& s);

	bool AddFileToProtectedList();

};

