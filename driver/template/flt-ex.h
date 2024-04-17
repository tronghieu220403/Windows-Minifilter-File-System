#pragma once

#include "../process/ps-monitor.h"

namespace flt
{
	bool IsTrustedRequestor(PFLT_CALLBACK_DATA data);

	String<char> DebugIrpFlags(ULONG flag);

	String<char> DebugIopbMajorFunc(UCHAR mj);
}