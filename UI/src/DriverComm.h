#ifndef DRIVERCOMM_H
#define DRIVERCOMM_H

#include <Windows.h>
#include <string>
#include <vector>

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

struct IOCTL_CMD
{
    IOCTL_CMD_CLASS cmd_class;
    size_t data_len;
    char data[1];
};

#define IOCTL_HIEU CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2204, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define HIEU_DEVICE_LINK L"\\\\.\\HieuDeviceLink"

class DriverComm {
public:
    DriverComm();
	DriverComm(const std::wstring& device_link);

    bool Initialize(const std::wstring& deviceLink);

    void Cleanup();

    void HideFile(const std::wstring& path);
    void HideDir(const std::wstring& path);
	void UnhideFile(const std::wstring& path);
	void UnhideDir(const std::wstring& path);

    void ProtectFile(const std::wstring& path);
    void ProtectDir(const std::wstring& path);
    void UnprotectFile(const std::wstring& path);
    void UnprotectDir(const std::wstring& path);

    void ProtectProcImage(const std::wstring& path);
    void UnprotectProcImage(const std::wstring& path);

	void WhitelistProcImage(const std::wstring& path);
	void UnwhitelistProcImage(const std::wstring& path);

	void EnableDriver();
	void DisableDriver();

	void EnableFileHide();
	void DisableFileHide();

	void EnableFileProtect();
	void DisableFileProtect();

	void EnableProcProtect();
	void DisableProcProtect();

private:
    HANDLE device;
    BOOL status;

    std::wstring GetDosPath(const std::wstring& wstr);

	bool SendCommand(IOCTL_CMD_CLASS cmdClass, const std::wstring& path);
};

#endif // DRIVERCOMM_H
