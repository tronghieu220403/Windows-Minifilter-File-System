#pragma once

#include "../Windows-Minifilter-Template/std/lib/vector/vector.h"
#include "../Windows-Minifilter-Template/std/lib/string/string.h"

#include <wdm.h>
#include <fltKernel.h>

Vector<String<char>> kAntiList;

namespace AntiDelete
{
	static void Register();

	static FLT_PREOP_CALLBACK_STATUS PreOperation(_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, _Flt_CompletionContext_Outptr_ PVOID* CompletionContext);

	static bool IsProtectedFile();

	static bool AddFileToProtectedList();

};

