#pragma once

#include "flt-ex.h"

namespace flt
{
	bool IsTrustedRequestor(PFLT_CALLBACK_DATA data)
	{
		if (data->RequestorMode == KernelMode || process::IsTrustedProcess((size_t)PsGetCurrentProcessId()) == true)
		{
			return true;
		}
		return false;
	}
}