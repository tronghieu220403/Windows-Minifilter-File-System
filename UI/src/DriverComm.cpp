#include "DriverComm.h"

DriverComm::DriverComm() : device(INVALID_HANDLE_VALUE), status(FALSE) {}

DriverComm::DriverComm(const std::wstring& device_link)
{
	this->Initialize(device_link);
}

bool DriverComm::Initialize(const std::wstring& device_link) {
    device = CreateFileW(device_link.c_str(), GENERIC_WRITE | GENERIC_READ | GENERIC_EXECUTE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
    if (device == INVALID_HANDLE_VALUE) {
        return false;
    }
    return true;
}

void DriverComm::Cleanup() {
    if (device != INVALID_HANDLE_VALUE) {
        CloseHandle(device);
        device = INVALID_HANDLE_VALUE;
    }
}

void DriverComm::HideFile(const std::wstring& path) {
    SendCommand(kHideFile, path);
}

void DriverComm::HideDir(const std::wstring& path) {
    SendCommand(kHideDir, path);
}

void DriverComm::UnhideFile(const std::wstring& path) {
	SendCommand(kUnhideFile, path);
}

void DriverComm::UnhideDir(const std::wstring& path) {
	SendCommand(kUnhideDir, path);
}

void DriverComm::ProtectFile(const std::wstring& path) {
    SendCommand(kProtectFile, path);
}

void DriverComm::ProtectDir(const std::wstring& path) {
    SendCommand(kProtectDir, path);
}

void DriverComm::UnprotectFile(const std::wstring& path) {
    SendCommand(kUnprotectFile, path);
}

void DriverComm::UnprotectDir(const std::wstring& path) {
    SendCommand(kUnprotectDir, path);
}

void DriverComm::ProtectProcImage(const std::wstring& path) {
    SendCommand(kProtectProcImage, path);
}

void DriverComm::UnprotectProcImage(const std::wstring& path) {
    SendCommand(kUnprotectProcImage, path);
}

void DriverComm::WhitelistProcImage(const std::wstring& path) {
	SendCommand(kWhitelistProcImage, path);
}

void DriverComm::UnwhitelistProcImage(const std::wstring& path) {
	SendCommand(kUnwhitelistProcImage, path);
}

void DriverComm::EnableDriver() {
	SendCommand(kEnableDriver, L"");
}

void DriverComm::DisableDriver() {
	SendCommand(kDisableDriver, L"");
}

void DriverComm::EnableFileHide() {
	SendCommand(kEnableFileHide, L"");
}

void DriverComm::DisableFileHide() {
	SendCommand(kDisableFileHide, L"");
}

void DriverComm::EnableFileProtect() {
	SendCommand(kEnableFileProtect, L"");
}

void DriverComm::DisableFileProtect() {
	SendCommand(kDisableFileProtect, L"");
}

void DriverComm::EnableProcProtect() {
	SendCommand(kEnableProcProtect, L"");
}

void DriverComm::DisableProcProtect() {
	SendCommand(kDisableProcProtect, L"");
}

std::wstring DriverComm::GetDosPath(const std::wstring& wstr) {
    std::wstring device_name = wstr.substr(0, wstr.find_first_of(L'\\'));
    std::wstring dos_name;
    dos_name.resize(128);
    DWORD status;
    while (QueryDosDeviceW(device_name.data(), (WCHAR*)dos_name.data(), dos_name.size()) == 0) {
        status = GetLastError();
        if (status != ERROR_INSUFFICIENT_BUFFER) {
            return std::wstring();
        }
        dos_name.resize(dos_name.size() * 2);
    }
    dos_name.resize(wcslen(dos_name.data()));

    return dos_name + wstr.substr(wstr.find_first_of(L'\\'));
}

void DriverComm::SendCommand(IOCTL_CMD_CLASS cmdClass, const std::wstring& path) {
    DWORD bytes_returned = 0;
    CHAR out_buffer[128] = { 0 };

    std::wstring dos_path = GetDosPath(path);
    std::vector<char> in_buffer(sizeof(IOCTL_CMD) + dos_path.size() * sizeof(WCHAR));

    IOCTL_CMD* cmd = (IOCTL_CMD*)&in_buffer[0];
    cmd->cmd_class = cmdClass;
    cmd->data_len = dos_path.size();
    memcpy(cmd->data, dos_path.data(), dos_path.size() * sizeof(WCHAR));

    status = DeviceIoControl(device, IOCTL_HIEU, &in_buffer[0], in_buffer.size() * sizeof(char), out_buffer, sizeof(out_buffer), &bytes_returned, (LPOVERLAPPED)NULL);
}
