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
		kHideReg = 0x80002008,
		kUnhideReg = 0x80002009,
		kHideProcId = 0x8000200A,
		kUnhideProcId = 0x8000200B,
		kProtectProcId = 0x8000200C,
		kUnprotectProcId = 0x8000200D,
		kHideProcImage = 0x8000200E,
		kUnhideProcImage = 0x80002010,
		kProtectProcImage = 0x80002011,
		kUnprotectProcImage = 0x80002012
};

struct IOCTL_CMD
{
	IOCTL_CMD_CLASS cmd_class;
	size_t data_len;
	char data[1];
};