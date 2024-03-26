#include "ioctl.h"

NTSTATUS ioctl::HandleIoctl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION stackLoc = IoGetCurrentIrpStackLocation(Irp);
	stackLoc = IoGetCurrentIrpStackLocation(Irp);

	PIOCTL_CMD cmd = (PIOCTL_CMD)Irp->AssociatedIrp.SystemBuffer;

	switch (stackLoc->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_CMD_CLASS::kHideFile:
		break;
	case IOCTL_CMD_CLASS::kHideDir:
		break;
	case IOCTL_CMD_CLASS::kUnhideFile:
		break;
	case IOCTL_CMD_CLASS::kUnhideDir:
		break;
	case IOCTL_CMD_CLASS::kHideProc:
		break;
	case IOCTL_CMD_CLASS::kUnhideProc:
		break;
	case IOCTL_CMD_CLASS::kHideReg:
		break;
	case IOCTL_CMD_CLASS::kUnhideReg:
		break;
	case IOCTL_CMD_CLASS::kProctectFile:
		break;
	case IOCTL_CMD_CLASS::kUnproctectFile:
		break;
	case IOCTL_CMD_CLASS::kProctectDir:
		break;
	case IOCTL_CMD_CLASS::kUnproctectDir:
		break;
	default:
		break;
	}

	return NTSTATUS();
}
