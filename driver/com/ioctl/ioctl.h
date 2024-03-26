#pragma once

#include "std/memory/memory.h"
#include "std/string/string.h"
#include "std/vector/vector.h"
#include "std/sync/mutex.h"

#include "process/ps-monitor.h"
#include "template/register.h"
#include "template/flt-ex.h"
#include "function/protect/file-protect.h"
#include "function/protect/proc-protect.h"
#include "function/hide/hide-file.h"
#include "function/hide/hide-proc.h"
#include "common.h"

namespace ioctl
{

	NTSTATUS HandleIoctl(PDEVICE_OBJECT DeviceObject, PIRP Irp);


}
