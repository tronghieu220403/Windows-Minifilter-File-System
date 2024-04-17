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

	String<char> DebugIrpFlags(ULONG flag)
	{
		String<char> str;
		if (FlagOn(flag, IRP_NOCACHE))
		{
			str += String<char>("IRP_NOCACHE");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_PAGING_IO))
		{
			str += String<char>("IRP_PAGING_IO");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_MOUNT_COMPLETION))
		{
			str += String<char>("IRP_MOUNT_COMPLETION");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_SYNCHRONOUS_API))
		{
			str += String<char>("IRP_SYNCHRONOUS_API");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_ASSOCIATED_IRP))
		{
			str += String<char>("IRP_ASSOCIATED_IRP");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_BUFFERED_IO))
		{
			str += String<char>("IRP_BUFFERED_IO");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_DEALLOCATE_BUFFER))
		{
			str += String<char>("IRP_DEALLOCATE_BUFFER");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_INPUT_OPERATION))
		{
			str += String<char>("IRP_INPUT_OPERATION");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_SYNCHRONOUS_PAGING_IO))
		{
			str += String<char>("IRP_SYNCHRONOUS_PAGING_IO");
			str += String<char>(", ");
		}

		if (FlagOn(flag, IRP_CREATE_OPERATION))
		{
			str += String<char>("IRP_CREATE_OPERATION");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_READ_OPERATION))
		{
			str += String<char>("IRP_READ_OPERATION");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_WRITE_OPERATION))
		{
			str += String<char>("IRP_WRITE_OPERATION");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_CLOSE_OPERATION))
		{
			str += String<char>("IRP_CLOSE_OPERATION");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_DEFER_IO_COMPLETION))
		{
			str += String<char>("IRP_DEFER_IO_COMPLETION");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_OB_QUERY_NAME))
		{
			str += String<char>("IRP_OB_QUERY_NAME");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_HOLD_DEVICE_QUEUE))
		{
			str += String<char>("IRP_HOLD_DEVICE_QUEUE");
			str += String<char>(", ");
		}
		if (FlagOn(flag, IRP_UM_DRIVER_INITIATED_IO))
		{
			str += String<char>("IRP_UM_DRIVER_INITIATED_IO");
			str += String<char>(", ");
		}

		return str;
	}

	String<char> DebugIopbMajorFunc(UCHAR mj)
	{
		String<char> str;
		switch (mj)
		{
		case IRP_MJ_WRITE:
			DebugMessage("IRP_MJ_WRITE");
			break;
		case IRP_MJ_QUERY_INFORMATION:
			DebugMessage("IRP_MJ_QUERY_INFORMATION");
			break;
		case IRP_MJ_SET_INFORMATION:
			DebugMessage("IRP_MJ_SET_INFORMATION");
			break;
		default:
			DebugMessage("%d", mj);
			break;
		}
		return str;
	}
}