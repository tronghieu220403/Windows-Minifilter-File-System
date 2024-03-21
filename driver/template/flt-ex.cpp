#pragma once

#include "flt-ex.h"

namespace flt
{
	bool IsTrustedRequestor(PFLT_CALLBACK_DATA data, size_t pid)
	{
		if (data->RequestorMode == KernelMode || process::IsTrustedProcess(pid) == true)
		{
			return true;
		}
		return false;
	}
}