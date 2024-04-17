enum IOCTL_CMD_CLASS
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

struct IOCTL_CMD
{
	IOCTL_CMD_CLASS cmd_class;
	size_t data_len;
	char data[1];
};