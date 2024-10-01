#pragma once

#include "../../std/memory/memory.h"
#include "../../std/string/string.h"
#include "../../std/vector/vector.h"
#include "../../std/sync/mutex.h"

namespace ioctl
{
	enum IOCTL_CMD_CLASS
	{
		kHideFile = 0x80002000,
		kHideDir = 0x80002001,
		kUnhideFile = 0x80002002,
		kUnhideDir = 0x80002003,

		kProctectFile = 0x80002004,
		kUnproctectFile = 0x80002005,
		kProctectDir = 0x80002006,
		kUnproctectDir = 0x80002007,

		kProtectProcImage = 0x80002011,
		kUnprotectProcImage = 0x80002012,

		kWhitelistProcImage = 0x80002013,
		kUnwhitelistProcImage = 0x80002014,

		kEnableDriver = 0x80002015,
		kDisableDriver = 0x80002016,

		kEnableFileHide = 0x80002017,
		kDisableFileHide = 0x80002018,

		kEnableFileProtect = 0x80002019,
		kDisableFileProtect = 0x80002020,

		kEnableProcProtect = 0x80002021,
		kDisableProcProtect = 0x80002022,
	};

	template <typename T>
	struct IOCTL_CMD_STRING
	{
		String<WCHAR> path;
	};

	// Cấu trúc dùng để truyền giá trị số nguyên 1 hoặc 0
	struct IOCTL_CMD_INT
	{
		int value;
	};

	using IOCTL_CMD_HIDE_FILE = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_UNHIDE_FILE = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_HIDE_DIR = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_UNHIDE_DIR = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_PROTECT_FILE = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_UNPROTECT_FILE = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_PROTECT_DIR = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_UNPROTECT_DIR = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_PROTECT_PROC_IMAGE = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_UNPROTECT_PROC_IMAGE = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_WHITELIST_PROC_IMAGE = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_UNWHITELIST_PROC_IMAGE = IOCTL_CMD_STRING<WCHAR>;

	struct IOCTL_CMD
	{
		IOCTL_CMD_CLASS cmd_class;
		size_t data_len;
		char data[1];

		// Hàm parse cho chuỗi
		template <typename T>
		T ParseCommand(IOCTL_CMD_CLASS expected_class) const
		{
			if (cmd_class != expected_class)
			{
				return T();
			}
			String<WCHAR> path(data_len);
			::MemCopy(&path[0], (WCHAR*)data, data_len);
			return T{ path };
		}

		// Hàm parse cho số nguyên
		IOCTL_CMD_INT ParseCommandInt(IOCTL_CMD_CLASS expected_class) const
		{
			if (cmd_class != expected_class)
			{
				return IOCTL_CMD_INT();
			}
			IOCTL_CMD_INT cmd_int;
			::MemCopy((char*)&cmd_int.value, (char*)data, sizeof(int));
			return cmd_int;
		}

		IOCTL_CMD_HIDE_FILE ParseHideFile() { return ParseCommand<IOCTL_CMD_HIDE_FILE>(kHideFile); }
		IOCTL_CMD_UNHIDE_FILE ParseUnhideFile() { return ParseCommand<IOCTL_CMD_UNHIDE_FILE>(kUnhideFile); }
		IOCTL_CMD_HIDE_DIR ParseHideDir() { return ParseCommand<IOCTL_CMD_HIDE_DIR>(kHideDir); }
		IOCTL_CMD_UNHIDE_DIR ParseUnhideDir() { return ParseCommand<IOCTL_CMD_UNHIDE_DIR>(kUnhideDir); }
		IOCTL_CMD_PROTECT_FILE ParseProtectFile() { return ParseCommand<IOCTL_CMD_PROTECT_FILE>(kProctectFile); }
		IOCTL_CMD_UNPROTECT_FILE ParseUnprotectFile() { return ParseCommand<IOCTL_CMD_UNPROTECT_FILE>(kUnproctectFile); }
		IOCTL_CMD_PROTECT_DIR ParseProtectDir() { return ParseCommand<IOCTL_CMD_PROTECT_DIR>(kProctectDir); }
		IOCTL_CMD_UNPROTECT_DIR ParseUnprotectDir() { return ParseCommand<IOCTL_CMD_UNPROTECT_DIR>(kUnproctectDir); }
		IOCTL_CMD_PROTECT_PROC_IMAGE ParseProtectProcImage() { return ParseCommand<IOCTL_CMD_PROTECT_PROC_IMAGE>(kProtectProcImage); }
		IOCTL_CMD_UNPROTECT_PROC_IMAGE ParseUnprotectProcImage() { return ParseCommand<IOCTL_CMD_UNPROTECT_PROC_IMAGE>(kUnprotectProcImage); }
		IOCTL_CMD_WHITELIST_PROC_IMAGE ParseWhitelistProcImage() { return ParseCommand<IOCTL_CMD_WHITELIST_PROC_IMAGE>(kWhitelistProcImage); }
		IOCTL_CMD_UNWHITELIST_PROC_IMAGE ParseUnwhitelistProcImage() { return ParseCommand<IOCTL_CMD_UNWHITELIST_PROC_IMAGE>(kUnwhitelistProcImage); }

		IOCTL_CMD_INT ParseEnableDriver() { return ParseCommandInt(kEnableDriver); }
		IOCTL_CMD_INT ParseDisableDriver() { return ParseCommandInt(kDisableDriver); }
		IOCTL_CMD_INT ParseEnableFileHide() { return ParseCommandInt(kEnableFileHide); }
		IOCTL_CMD_INT ParseDisableFileHide() { return ParseCommandInt(kDisableFileHide); }
		IOCTL_CMD_INT ParseEnableFileProtect() { return ParseCommandInt(kEnableFileProtect); }
		IOCTL_CMD_INT ParseDisableFileProtect() { return ParseCommandInt(kDisableFileProtect); }
		IOCTL_CMD_INT ParseEnableProcProtect() { return ParseCommandInt(kEnableProcProtect); }
		IOCTL_CMD_INT ParseDisableProcProtect() { return ParseCommandInt(kDisableProcProtect); }
	};

	// Flatten các cấu trúc lệnh IOCTL cho chuỗi
	template <typename T>
	IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_STRING<T>* cmd, IOCTL_CMD_CLASS cmd_class)
	{
		IOCTL_CMD* ioctlCmd = (IOCTL_CMD*)new char[sizeof(IOCTL_CMD) + cmd->path.Size() * sizeof(WCHAR)];
		ioctlCmd->cmd_class = cmd_class;
		ioctlCmd->data_len = cmd->path.Size() * sizeof(WCHAR);
		::MemCopy((char*)ioctlCmd->data, (char*)cmd->path.Data(), ioctlCmd->data_len);
		return ioctlCmd;
	}

	// Flatten cho số nguyên
	inline IOCTL_CMD* FlattenIoctlCmdInt(IOCTL_CMD_INT* cmd, IOCTL_CMD_CLASS cmd_class)
	{
		IOCTL_CMD* ioctlCmd = (IOCTL_CMD*)new char[sizeof(IOCTL_CMD) + sizeof(int)];
		ioctlCmd->cmd_class = cmd_class;
		ioctlCmd->data_len = sizeof(int);
		::MemCopy(ioctlCmd->data, (char*)&cmd->value, sizeof(int));
		return ioctlCmd;
	}

	// Hàm cụ thể để flatten các lệnh Enable/Disable
	inline IOCTL_CMD* FlattenEnableDriver(bool enable)
	{
		IOCTL_CMD_INT cmd;
		cmd.value = enable ? 1 : 0;
		return FlattenIoctlCmdInt(&cmd, enable ? kEnableDriver : kDisableDriver);
	}

	inline IOCTL_CMD* FlattenEnableFileHide(bool enable)
	{
		IOCTL_CMD_INT cmd;
		cmd.value = enable ? 1 : 0;
		return FlattenIoctlCmdInt(&cmd, enable ? kEnableFileHide : kDisableFileHide);
	}

	inline IOCTL_CMD* FlattenEnableFileProtect(bool enable)
	{
		IOCTL_CMD_INT cmd;
		cmd.value = enable ? 1 : 0;
		return FlattenIoctlCmdInt(&cmd, enable ? kEnableFileProtect : kDisableFileProtect);
	}

	inline IOCTL_CMD* FlattenEnableProcProtect(bool enable)
	{
		IOCTL_CMD_INT cmd;
		cmd.value = enable ? 1 : 0;
		return FlattenIoctlCmdInt(&cmd, enable ? kEnableProcProtect : kDisableProcProtect);
	}

	// Các hàm Flatten cho chuỗi (file và thư mục)
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_HIDE_FILE* cmd) { return FlattenIoctlCmd(cmd, kHideFile); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_UNHIDE_FILE* cmd) { return FlattenIoctlCmd(cmd, kUnhideFile); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_HIDE_DIR* cmd) { return FlattenIoctlCmd(cmd, kHideDir); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_UNHIDE_DIR* cmd) { return FlattenIoctlCmd(cmd, kUnhideDir); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_PROTECT_FILE* cmd) { return FlattenIoctlCmd(cmd, kProctectFile); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_UNPROTECT_FILE* cmd) { return FlattenIoctlCmd(cmd, kUnproctectFile); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_PROTECT_DIR* cmd) { return FlattenIoctlCmd(cmd, kProctectDir); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_UNPROTECT_DIR* cmd) { return FlattenIoctlCmd(cmd, kUnproctectDir); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_PROTECT_PROC_IMAGE* cmd) { return FlattenIoctlCmd(cmd, kProtectProcImage); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_UNPROTECT_PROC_IMAGE* cmd) { return FlattenIoctlCmd(cmd, kUnprotectProcImage); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_WHITELIST_PROC_IMAGE* cmd) { return FlattenIoctlCmd(cmd, kWhitelistProcImage); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_UNWHITELIST_PROC_IMAGE* cmd) { return FlattenIoctlCmd(cmd, kUnwhitelistProcImage); }
}
