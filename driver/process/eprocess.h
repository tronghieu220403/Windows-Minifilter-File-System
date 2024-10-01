#pragma once

#include "../../std/string/string.h"
#include "../../std/vector/vector.h"
#include "../../std/sync/mutex.h"

#include "../../template/system_routine.h"
#include "../../template/debug.h"
#pragma warning( disable : 6001)

#define SYSTEM_PROCESS_ID 4

namespace eprocess
{
	size_t GetAplRva();
	size_t GetPidRva();

	inline size_t kPidRva = 0;
	inline size_t kAplRva = 0;

	void DrvRegister();

}
