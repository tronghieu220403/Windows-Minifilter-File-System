#pragma once

#include "std/memory/memory.h"
#include "std/string/string.h"
#include "std/vector/vector.h"
#include "std/sync/mutex.h"

namespace ioctl
{
	typedef enum IOCTL_CMD_CLASS
	{
		kHideFile = 0x80002000,
		kHideDir = 0x80002001,
		kUnhideFile = 0x80002002,
		kUnhideDir = 0x80002003,
		kHideProc = 0x80002004,
		kUnhideProc = 0x80002005,
		kHideReg = 0x80002006,
		kUnhideReg = 0x80002007,

		kProctectFile = 0x80002008,
		kUnproctectFile = 0x80002009,
		kProctectDir = 0x8000200A,
		kUnproctectDir = 0x8000200B
	};

	typedef struct IOCTL_CMD
	{
		IOCTL_CMD_CLASS cmd_class;
		ULONG data_len;
		char data[1];

		inline IOCTL_CMD_HIDE_FILE ParseHideFile()
		{
			if (cmd_class != IOCTL_CMD_CLASS::kHideFile)
			{
				return IOCTL_CMD_HIDE_FILE();
			}
			String<WCHAR> file_path(data_len);
			::MemCopy(&file_path[0], (WCHAR*)data, data_len);
			return IOCTL_CMD_HIDE_FILE{ file_path };
		};

		inline IOCTL_CMD_UNHIDE_FILE ParseUnhideFile()
		{
			if (cmd_class != IOCTL_CMD_CLASS::kUnhideFile)
			{
				return IOCTL_CMD_UNHIDE_FILE();
			}
			String<WCHAR> file_path(data_len);
			::MemCopy(&file_path[0], (WCHAR*)data, data_len);
			return IOCTL_CMD_UNHIDE_FILE{ file_path };
		}

		IOCTL_CMD_HIDE_DIR ParseHideDir()
		{
			if (cmd_class != IOCTL_CMD_CLASS::kHideDir)
			{
				return IOCTL_CMD_HIDE_DIR();
			}
			String<WCHAR> dir_path(data_len);
			::MemCopy(&dir_path[0], (WCHAR*)data, data_len);
			return IOCTL_CMD_HIDE_DIR{ dir_path };
		}

		IOCTL_CMD_UNHIDE_DIR ParseUnhideDir()
		{
			if (cmd_class != IOCTL_CMD_CLASS::kUnhideDir)
			{
				return IOCTL_CMD_UNHIDE_DIR();
			}
			String<WCHAR> dir_path(data_len);
			::MemCopy(&dir_path[0], (WCHAR*)data, data_len);
			return IOCTL_CMD_UNHIDE_DIR{ dir_path };
		}

		IOCTL_CMD_HIDE_PROC ParseHideProc()
		{
			if (cmd_class != IOCTL_CMD_CLASS::kHideProc)
			{
				return IOCTL_CMD_HIDE_PROC();
			}
			ULONG pid = *(ULONG*)data;
			return IOCTL_CMD_HIDE_PROC{ pid };
		}

		IOCTL_CMD_UNHIDE_PROC ParseUnhideProc()
		{
			if (cmd_class != IOCTL_CMD_CLASS::kUnhideProc)
			{
				return IOCTL_CMD_UNHIDE_PROC();
			}
			ULONG pid = *(ULONG*)data;
			return IOCTL_CMD_UNHIDE_PROC{ pid };
		}

		IOCTL_CMD_HIDE_REG ParseHideReg()
		{
			if (cmd_class != IOCTL_CMD_CLASS::kHideReg)
			{
				return IOCTL_CMD_HIDE_REG();
			}
			String<WCHAR> reg_path(data_len);
			::MemCopy(&reg_path[0], (WCHAR*)data, data_len);
			return IOCTL_CMD_HIDE_REG{ reg_path };
		}

		IOCTL_CMD_UNHIDE_REG ParseUnhideReg()
		{
			if (cmd_class != IOCTL_CMD_CLASS::kUnhideReg)
			{
				return IOCTL_CMD_UNHIDE_REG();
			}
			String<WCHAR> reg_path(data_len);
			::MemCopy(&reg_path[0], (WCHAR*)data, data_len);
			return IOCTL_CMD_UNHIDE_REG{ reg_path };
		}

		IOCTL_CMD_PROTECT_FILE ParseProtectFile()
		{
			if (cmd_class != IOCTL_CMD_CLASS::kProctectFile)
			{
				return IOCTL_CMD_PROTECT_FILE();
			}
			String<WCHAR> file_path(data_len);
			::MemCopy(&file_path[0], (WCHAR*)data, data_len);
			return IOCTL_CMD_PROTECT_FILE{ file_path };
		}

		IOCTL_CMD_UNPROTECT_FILE ParseUnprotectFile()
		{
			if (cmd_class != IOCTL_CMD_CLASS::kUnproctectFile)
			{
				return IOCTL_CMD_UNPROTECT_FILE();
			}
			String<WCHAR> file_path(data_len);
			::MemCopy(&file_path[0], (WCHAR*)data, data_len);
			return IOCTL_CMD_UNPROTECT_FILE{ file_path };
		}

		IOCTL_CMD_PROTECT_DIR ParseProtectDir()
		{
			if (cmd_class != IOCTL_CMD_CLASS::kProctectDir)
			{
				return IOCTL_CMD_PROTECT_DIR();
			}
			String<WCHAR> dir_path(data_len);
			::MemCopy(&dir_path[0], (WCHAR*)data, data_len);
			return IOCTL_CMD_PROTECT_DIR{ dir_path };
		}

		IOCTL_CMD_UNPROTECT_DIR ParseUnprotectDir()
		{
			if (cmd_class != IOCTL_CMD_CLASS::kUnproctectDir)
			{
				return IOCTL_CMD_UNPROTECT_DIR();
			}
			String<WCHAR> dir_path(data_len);
			::MemCopy(&dir_path[0], (WCHAR*)data, data_len);
			return IOCTL_CMD_UNPROTECT_DIR{ dir_path };
		}

	} IOCTL_CMD, * PIOCTL_CMD;

	typedef struct IOCTL_CMD_HIDE_FILE
	{
		String<WCHAR> file_path;
	} IOCTL_CMD_HIDE_FILE, * PIOCTL_CMD_HIDE_FILE;

	typedef struct IOCTL_CMD_UNHIDE_FILE
	{
		String<WCHAR> file_path;
	} IOCTL_CMD_HIDE_FILE, * PIOCTL_CMD_HIDE_FILE;

	typedef struct IOCTL_CMD_HIDE_DIR
	{
		String<WCHAR> dir_path;
	} IOCTL_CMD_HIDE_DIR, * PIOCTL_CMD_HIDE_DIR;

	typedef struct IOCTL_CMD_UNHIDE_DIR
	{
		String<WCHAR> dir_path;
	} IOCTL_CMD_UNHIDE_DIR, * PIOCTL_CMD_UNHIDE_DIR;

	typedef struct IOCTL_CMD_HIDE_PROC
	{
		ULONG pid;
	} IOCTL_CMD_HIDE_PROC, * PIOCTL_CMD_HIDE_PROC;

	typedef struct IOCTL_CMD_UNHIDE_PROC
	{
		ULONG pid;
	} IOCTL_CMD_UNHIDE_PROC, * PIOCTL_CMD_UNHIDE_PROC;

	typedef struct IOCTL_CMD_HIDE_REG
	{
		String<WCHAR> reg_path;
	} IOCTL_CMD_HIDE_REG, * PIOCTL_CMD_HIDE_REG;

	typedef struct IOCTL_CMD_UNHIDE_REG
	{
		String<WCHAR> reg_path;
	} IOCTL_CMD_UNHIDE_REG, * PIOCTL_CMD_UNHIDE_REG;

	typedef struct IOCTL_CMD_PROTECT_FILE
	{
		String<WCHAR> file_path;
	} IOCTL_CMD_PROTECT_FILE, * PIOCTL_CMD_PROTECT_FILE;

	typedef struct IOCTL_CMD_UNPROTECT_FILE
	{
		String<WCHAR> file_path;
	} IOCTL_CMD_UNPROTECT_FILE, * PIOCTL_CMD_UNPROTECT_FILE;

	typedef struct IOCTL_CMD_PROTECT_DIR
	{
		String<WCHAR> dir_path;
	} IOCTL_CMD_PROTECT_DIR, * PIOCTL_CMD_PROTECT_DIR;

	typedef struct IOCTL_CMD_UNPROTECT_DIR
	{
		String<WCHAR> dir_path;
	} IOCTL_CMD_UNPROTECT_DIR, * PIOCTL_CMD_UNPROTECT_DIR;

	/*
	Write muliple functions to flatten all the above structures of IOCTL_CMD_CLASS into an IOCTL_CMD consecutive array of char, so that I can send it through a communication port in kernel mode driver. The function take only one parameter: the IOCTL_CMD_xxx structure. First we must allocate memory for IOCTL_CMD structure (the remaining data is write right after the data[1]). Return a pointer to that IOCTL_CMD.
	*/


}