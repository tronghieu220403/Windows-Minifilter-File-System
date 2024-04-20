#include <iostream>
#include <Windows.h>

#include "common.h"

#include <string>
#include <vector>

using namespace std;

#define IOCTL_HIEU CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2204, METHOD_BUFFERED, FILE_ANY_ACCESS)

HANDLE device = INVALID_HANDLE_VALUE;
BOOL status = FALSE;

wstring GetDosPath(const wstring* wstr)
{
    wstring device_name = wstr->substr(0, wstr->find_first_of('\\'));
    wstring dos_name;
    dos_name.resize(128);
    DWORD status;
    while (QueryDosDeviceW(device_name.data(), (WCHAR*)dos_name.data(), dos_name.size()) == 0)
    {
        status = GetLastError();
        if (status != ERROR_INSUFFICIENT_BUFFER)
        {
            return wstring();
        }
        dos_name.resize(dos_name.size() * 2);
    }
    dos_name.resize(wcslen(dos_name.data()));

    return dos_name + wstr->substr(wstr->find_first_of('\\'));
}

void ProtectFile(wstring path)
{
    DWORD bytes_returned = 0;
    CHAR out_buffer[128] = { 0 };

    wstring dos_path = GetDosPath(&path);

    cout << dos_path.size() << endl;
    wcout << dos_path << endl;

    vector<char> in_buffer(sizeof(IOCTL_CMD) + dos_path.size() * sizeof(WCHAR));

    IOCTL_CMD* cmd = (IOCTL_CMD*)&in_buffer[0];
    cmd->cmd_class = kProctectFile;
    cmd->data_len = dos_path.size();
    memcpy(cmd->data, &dos_path[0], dos_path.size() * sizeof(WCHAR));

    wcout << dos_path << endl;
    wcout << (WCHAR*)(cmd->data) << endl;
    cout << in_buffer.size();

    status = DeviceIoControl(device, IOCTL_HIEU, &in_buffer[0], in_buffer.size() * sizeof(char), out_buffer, sizeof(out_buffer), &bytes_returned, (LPOVERLAPPED)NULL);
}


void ProtectDir(wstring path)
{
    DWORD bytes_returned = 0;
    CHAR out_buffer[128] = { 0 };

    wstring dos_path = GetDosPath(&path);

    cout << dos_path.size() << endl;
    wcout << dos_path << endl;

    vector<char> in_buffer(sizeof(IOCTL_CMD) + dos_path.size() * sizeof(WCHAR));

    IOCTL_CMD* cmd = (IOCTL_CMD*)&in_buffer[0];
    cmd->cmd_class = kProctectDir;
    cmd->data_len = dos_path.size();
    memcpy(cmd->data, &dos_path[0], dos_path.size() * sizeof(WCHAR));

    wcout << dos_path << endl;
    wcout << (WCHAR*)(cmd->data) << endl;
    cout << in_buffer.size();

    status = DeviceIoControl(device, IOCTL_HIEU, &in_buffer[0], in_buffer.size() * sizeof(char), out_buffer, sizeof(out_buffer), &bytes_returned, (LPOVERLAPPED)NULL);
}

void HideFile(wstring path)
{
    DWORD bytes_returned = 0;
    CHAR out_buffer[128] = { 0 };

    wstring dos_path = GetDosPath(&path);

    cout << dos_path.size() << endl;
    wcout << dos_path << endl;

    vector<char> in_buffer(sizeof(IOCTL_CMD) + dos_path.size() * sizeof(WCHAR));

    IOCTL_CMD* cmd = (IOCTL_CMD*)&in_buffer[0];
    cmd->cmd_class = kHideFile;
    cmd->data_len = dos_path.size();
    memcpy(cmd->data, &dos_path[0], dos_path.size() * sizeof(WCHAR));

    wcout << dos_path << endl;
    wcout << (WCHAR*)(cmd->data) << endl;
    cout << in_buffer.size();

    status = DeviceIoControl(device, IOCTL_HIEU, &in_buffer[0], in_buffer.size() * sizeof(char), out_buffer, sizeof(out_buffer), &bytes_returned, (LPOVERLAPPED)NULL);
}

void HideDir(wstring path)
{
    DWORD bytes_returned = 0;
    CHAR out_buffer[128] = { 0 };

    wstring dos_path = GetDosPath(&path);

    cout << dos_path.size() << endl;
    wcout << dos_path << endl;

    vector<char> in_buffer(sizeof(IOCTL_CMD) + dos_path.size() * sizeof(WCHAR));

    IOCTL_CMD* cmd = (IOCTL_CMD*)&in_buffer[0];
    cmd->cmd_class = kHideDir;
    cmd->data_len = dos_path.size();
    memcpy(cmd->data, &dos_path[0], dos_path.size() * sizeof(WCHAR));

    wcout << dos_path << endl;
    wcout << (WCHAR*)(cmd->data) << endl;
    cout << in_buffer.size();

    status = DeviceIoControl(device, IOCTL_HIEU, &in_buffer[0], in_buffer.size() * sizeof(char), out_buffer, sizeof(out_buffer), &bytes_returned, (LPOVERLAPPED)NULL);
}


int main(char argc, char** argv)
{

    device = CreateFileW(L"\\\\.\\HieuDeviceLink", GENERIC_WRITE | GENERIC_READ | GENERIC_EXECUTE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);



    if (device == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    HideFile(L"C:\\Users\\hieu\\source\\repos\\hide.txt");
    HideDir(L"C:\\Users\\hieu\\source\\repos\\DriverHelloWorld");
    ProtectDir(L"C:\\Users\\hieu\\source\\repos\\ConsoleApplication1");

    CloseHandle(device);
}