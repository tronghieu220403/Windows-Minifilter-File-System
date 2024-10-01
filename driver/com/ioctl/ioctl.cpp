#include "ioctl.h"

NTSTATUS ioctl::DrvRegister(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path)
{
	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ioctl::HandleIoctl;

	// routines that will execute once a handle to our device's symbolik link is opened/closed
	driver_object->MajorFunction[IRP_MJ_CREATE] = ioctl::MajorFunction;
	driver_object->MajorFunction[IRP_MJ_CLOSE] = ioctl::MajorFunction;

	NTSTATUS status = IoCreateDevice(driver_object, 0, &DEVICE_NAME, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &driver_object->DeviceObject);

	if (!NT_SUCCESS(status))
	{
		DebugMessage("Could not create device %wZ", DEVICE_NAME);
		return STATUS_UNSUCCESSFUL;
	}
	else
	{
		DebugMessage("Device %wZ created", DEVICE_NAME);
	}

	status = IoCreateSymbolicLink(&DEVICE_SYMBOLIC_NAME, &DEVICE_NAME);
	if (NT_SUCCESS(status))
	{
		DebugMessage("Symbolic link %wZ created", DEVICE_SYMBOLIC_NAME);
	}
	else
	{
		DebugMessage("Error creating symbolic link %wZ", DEVICE_SYMBOLIC_NAME);
		return STATUS_UNSUCCESSFUL;
	}
	return STATUS_SUCCESS;
}

NTSTATUS ioctl::DrvUnload(PDRIVER_OBJECT driver_object)
{
	DebugMessage("DriverUnload: Unload\n");

	IoDeleteDevice(driver_object->DeviceObject);
	IoDeleteSymbolicLink(&DEVICE_SYMBOLIC_NAME);

	return STATUS_SUCCESS;
}

NTSTATUS ioctl::HandleIoctl(PDEVICE_OBJECT device_object, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_object);

	PIO_STACK_LOCATION stack_loc = IoGetCurrentIrpStackLocation(irp);
	stack_loc = IoGetCurrentIrpStackLocation(irp);

	IOCTL_CMD* cmd = (IOCTL_CMD*)irp->AssociatedIrp.SystemBuffer;

	if (stack_loc->Parameters.DeviceIoControl.IoControlCode != IOCTL_HIEU)
	{
		return STATUS_UNSUCCESSFUL;
	}

	IOCTL_CMD_CLASS type = cmd->cmd_class;
	// TODO: Build a function to handle each type
	String<WCHAR> str;

	switch (type)
	{
	case IOCTL_CMD_CLASS::kHideFile:

		str = cmd->ParseHideFile().path;
		DebugMessage("Hide file: %ws", str.Data());
		hide_file::AddFileToHideList(&str);
		break;

	case IOCTL_CMD_CLASS::kHideDir:

		str = cmd->ParseHideDir().path;
		DebugMessage("Hide dir: %ws", str.Data());
		hide_file::AddDirToHideList(&str);
		break;

	case IOCTL_CMD_CLASS::kUnhideFile:

		str = cmd->ParseUnhideFile().path;
		DebugMessage("Unhide file: %ws", str.Data());
		hide_file::DeleteFileFromHideList(&str);
		break;

	case IOCTL_CMD_CLASS::kUnhideDir:

		str = cmd->ParseUnhideDir().path;
		DebugMessage("Unhide dir: %ws", str.Data());
		hide_file::DeleteDirFromHideList(&str);
		break;

	case IOCTL_CMD_CLASS::kProtectFile:

		str = cmd->ParseProtectFile().path;
		DebugMessage("Protect file: %ws", str.Data());
		protect_file::AddFileToProtectedList(&str);
		break;

	case IOCTL_CMD_CLASS::kUnprotectFile:

		str = cmd->ParseUnprotectFile().path;
		DebugMessage("Unprotect file: %ws", str.Data());
		protect_file::RemoveFileFromProtectedList(&str);
		break;

	case IOCTL_CMD_CLASS::kProtectDir:

		str = cmd->ParseProtectDir().path;
		DebugMessage("Protect dir: %ws", str.Data());
		protect_file::AddDirToProtectedList(&str);
		break;

	case IOCTL_CMD_CLASS::kUnprotectDir:

		str = cmd->ParseUnprotectDir().path;
		DebugMessage("Unprotect dir: %ws", str.Data());
		protect_file::RemoveDirFromProtectedList(&str);
		break;

	case IOCTL_CMD_CLASS::kProtectProcImage:

		str = cmd->ParseProtectProcImage().path;
		DebugMessage("Protect proc image: %ws", str.Data());
		protect_proc::AddImageToProtectList(str);
		break;

	case IOCTL_CMD_CLASS::kUnprotectProcImage:

		str = cmd->ParseUnprotectProcImage().path;
		DebugMessage("Unprotect proc image: %ws", str.Data());
		protect_proc::DeleteImageFromProtectList(str);
		break;

	case IOCTL_CMD_CLASS::kWhitelistProcImage:

		str = cmd->ParseWhitelistProcImage().path;
		DebugMessage("Whitelist proc image: %ws", str.Data());
		process::AddTrustedProcess(str);
		break;

	case IOCTL_CMD_CLASS::kUnwhitelistProcImage:

		str = cmd->ParseUnwhitelistProcImage().path;
		DebugMessage("Unwhitelist proc image: %ws", str.Data());
		process::RemoveTrustedProcess(str);
		break;

	case IOCTL_CMD_CLASS::kEnableDriver:
		DebugMessage("Enable driver");
		// DO SOMETHING
		break;

	case IOCTL_CMD_CLASS::kDisableDriver:
		DebugMessage("Disable driver");
		// DO SOMETHING
		break;

	case IOCTL_CMD_CLASS::kEnableFileProtect:
		protect_file::kEnableProtectFile = true;
		DebugMessage("Enable protect file");
		break;

	case IOCTL_CMD_CLASS::kDisableFileProtect:
		protect_file::kEnableProtectFile = false;
		DebugMessage("Disable protect file");
		break;

	case IOCTL_CMD_CLASS::kEnableFileHide:
		hide_file::kEnableHideFile = true;
		DebugMessage("Enable hide file");

	case IOCTL_CMD_CLASS::kDisableFileHide:
		hide_file::kEnableHideFile = false;
		DebugMessage("Disable hide file");
		break;

	case IOCTL_CMD_CLASS::kEnableProcProtect:
		protect_proc::kEnableProcProtect = true;
		DebugMessage("Enable protect process");
		break;

	case IOCTL_CMD_CLASS::kDisableProcProtect:
		protect_proc::kEnableProcProtect = false;
		DebugMessage("Disable protect process");
		break;

	default:
		break;
	}

	irp->IoStatus.Status = STATUS_SUCCESS;

	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS ioctl::MajorFunction(PDEVICE_OBJECT device_object, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_object);

	PIO_STACK_LOCATION stackLocation = NULL;
	stackLocation = IoGetCurrentIrpStackLocation(irp);

	switch (stackLocation->MajorFunction)
	{
	case IRP_MJ_CREATE:
		DebugMessage("Handle to symbolink link %wZ opened", DEVICE_SYMBOLIC_NAME);
		break;
	case IRP_MJ_CLOSE:
		DebugMessage("Handle to symbolink link %wZ closed", DEVICE_SYMBOLIC_NAME);
		break;
	default:
		break;
	}

	irp->IoStatus.Information = 0;
	irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;

}