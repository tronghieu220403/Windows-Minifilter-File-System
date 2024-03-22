#pragma once

#include "flt-ex.h"

namespace flt
{
	bool IsTrustedRequestor(PFLT_CALLBACK_DATA data)
	{
		if (data->RequestorMode == KernelMode)
		{
			return true;
		}

		if (process::IsTrustedProcess((size_t)PsGetProcessId(IoThreadToProcess(data->Thread))) == true)
		{
			return true;
		}

		return false;
	}
}