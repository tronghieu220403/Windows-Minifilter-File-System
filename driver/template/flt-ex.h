#pragma once

#include "../process/ps-monitor.h"

namespace flt
{
	bool IsTrustedRequestor(PFLT_CALLBACK_DATA data, size_t pid);
}