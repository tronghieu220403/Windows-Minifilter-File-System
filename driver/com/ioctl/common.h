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

		kProtectFile = 0x80002004,
		kUnprotectFile = 0x80002005,
		kProtectDir = 0x80002006,
		kUnprotectDir = 0x80002007,

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
	using IOCTL_CMD_ENABLE_DRIVER = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_DISABLE_DRIVER = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_ENABLE_FILE_HIDE = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_DISABLE_FILE_HIDE = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_ENABLE_FILE_PROTECT = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_DISABLE_FILE_PROTECT = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_ENABLE_PROC_PROTECT = IOCTL_CMD_STRING<WCHAR>;
	using IOCTL_CMD_DISABLE_PROC_PROTECT = IOCTL_CMD_STRING<WCHAR>;

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

		IOCTL_CMD_HIDE_FILE ParseHideFile() { return ParseCommand<IOCTL_CMD_HIDE_FILE>(kHideFile); }
		IOCTL_CMD_UNHIDE_FILE ParseUnhideFile() { return ParseCommand<IOCTL_CMD_UNHIDE_FILE>(kUnhideFile); }
		IOCTL_CMD_HIDE_DIR ParseHideDir() { return ParseCommand<IOCTL_CMD_HIDE_DIR>(kHideDir); }
		IOCTL_CMD_UNHIDE_DIR ParseUnhideDir() { return ParseCommand<IOCTL_CMD_UNHIDE_DIR>(kUnhideDir); }
		IOCTL_CMD_PROTECT_FILE ParseProtectFile() { return ParseCommand<IOCTL_CMD_PROTECT_FILE>(kProtectFile); }
		IOCTL_CMD_UNPROTECT_FILE ParseUnprotectFile() { return ParseCommand<IOCTL_CMD_UNPROTECT_FILE>(kUnprotectFile); }
		IOCTL_CMD_PROTECT_DIR ParseProtectDir() { return ParseCommand<IOCTL_CMD_PROTECT_DIR>(kProtectDir); }
		IOCTL_CMD_UNPROTECT_DIR ParseUnprotectDir() { return ParseCommand<IOCTL_CMD_UNPROTECT_DIR>(kUnprotectDir); }
		IOCTL_CMD_PROTECT_PROC_IMAGE ParseProtectProcImage() { return ParseCommand<IOCTL_CMD_PROTECT_PROC_IMAGE>(kProtectProcImage); }
		IOCTL_CMD_UNPROTECT_PROC_IMAGE ParseUnprotectProcImage() { return ParseCommand<IOCTL_CMD_UNPROTECT_PROC_IMAGE>(kUnprotectProcImage); }
		IOCTL_CMD_WHITELIST_PROC_IMAGE ParseWhitelistProcImage() { return ParseCommand<IOCTL_CMD_WHITELIST_PROC_IMAGE>(kWhitelistProcImage); }
		IOCTL_CMD_UNWHITELIST_PROC_IMAGE ParseUnwhitelistProcImage() { return ParseCommand<IOCTL_CMD_UNWHITELIST_PROC_IMAGE>(kUnwhitelistProcImage); }
		IOCTL_CMD_ENABLE_DRIVER ParseEnableDriver() { return ParseCommand<IOCTL_CMD_ENABLE_DRIVER>(kEnableDriver); }
		IOCTL_CMD_DISABLE_DRIVER ParseDisableDriver() { return ParseCommand<IOCTL_CMD_DISABLE_DRIVER>(kDisableDriver); }
		IOCTL_CMD_ENABLE_FILE_HIDE ParseEnableFileHide() { return ParseCommand<IOCTL_CMD_ENABLE_FILE_HIDE>(kEnableFileHide); }
		IOCTL_CMD_DISABLE_FILE_HIDE ParseDisableFileHide() { return ParseCommand<IOCTL_CMD_DISABLE_FILE_HIDE>(kDisableFileHide); }
		IOCTL_CMD_ENABLE_FILE_PROTECT ParseEnableFileProtect() { return ParseCommand<IOCTL_CMD_ENABLE_FILE_PROTECT>(kEnableFileProtect); }
		IOCTL_CMD_DISABLE_FILE_PROTECT ParseDisableFileProtect() { return ParseCommand<IOCTL_CMD_DISABLE_FILE_PROTECT>(kDisableFileProtect); }
		IOCTL_CMD_ENABLE_PROC_PROTECT ParseEnableProcProtect() { return ParseCommand<IOCTL_CMD_ENABLE_PROC_PROTECT>(kEnableProcProtect); }
		IOCTL_CMD_DISABLE_PROC_PROTECT ParseDisableProcProtect() { return ParseCommand<IOCTL_CMD_DISABLE_PROC_PROTECT>(kDisableProcProtect); }
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

	// Các hàm Flatten cho chuỗi (file và thư mục)
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_HIDE_FILE* cmd) { return FlattenIoctlCmd(cmd, kHideFile); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_UNHIDE_FILE* cmd) { return FlattenIoctlCmd(cmd, kUnhideFile); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_HIDE_DIR* cmd) { return FlattenIoctlCmd(cmd, kHideDir); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_UNHIDE_DIR* cmd) { return FlattenIoctlCmd(cmd, kUnhideDir); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_PROTECT_FILE* cmd) { return FlattenIoctlCmd(cmd, kProtectFile); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_UNPROTECT_FILE* cmd) { return FlattenIoctlCmd(cmd, kUnprotectFile); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_PROTECT_DIR* cmd) { return FlattenIoctlCmd(cmd, kProtectDir); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_UNPROTECT_DIR* cmd) { return FlattenIoctlCmd(cmd, kUnprotectDir); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_PROTECT_PROC_IMAGE* cmd) { return FlattenIoctlCmd(cmd, kProtectProcImage); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_UNPROTECT_PROC_IMAGE* cmd) { return FlattenIoctlCmd(cmd, kUnprotectProcImage); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_WHITELIST_PROC_IMAGE* cmd) { return FlattenIoctlCmd(cmd, kWhitelistProcImage); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_UNWHITELIST_PROC_IMAGE* cmd) { return FlattenIoctlCmd(cmd, kUnwhitelistProcImage); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_ENABLE_DRIVER* cmd) { return FlattenIoctlCmd(cmd, kEnableDriver); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_DISABLE_DRIVER* cmd) { return FlattenIoctlCmd(cmd, kDisableDriver); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_ENABLE_FILE_HIDE* cmd) { return FlattenIoctlCmd(cmd, kEnableFileHide); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_DISABLE_FILE_HIDE* cmd) { return FlattenIoctlCmd(cmd, kDisableFileHide); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_ENABLE_FILE_PROTECT* cmd) { return FlattenIoctlCmd(cmd, kEnableFileProtect); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_DISABLE_FILE_PROTECT* cmd) { return FlattenIoctlCmd(cmd, kDisableFileProtect); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_ENABLE_PROC_PROTECT* cmd) { return FlattenIoctlCmd(cmd, kEnableProcProtect); }
	inline IOCTL_CMD* FlattenIoctlCmd(IOCTL_CMD_DISABLE_PROC_PROTECT* cmd) { return FlattenIoctlCmd(cmd, kDisableProcProtect); }
}
