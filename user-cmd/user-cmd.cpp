#include <iostream>
#include <Windows.h>

#include <string>

using namespace std;

#define IOCTL_HIEU CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2204, METHOD_BUFFERED, FILE_ANY_ACCESS)

int main(char argc, char** argv)
{
    HANDLE device = INVALID_HANDLE_VALUE;
    BOOL status = FALSE;
    DWORD bytes_returned = 0;
    CHAR out_buffer[128] = { 0 };
    wstring in_buffer;

    device = CreateFileW(L"\\\\.\\HieuDeviceLink", GENERIC_WRITE | GENERIC_READ | GENERIC_EXECUTE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);

    if (device == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    in_buffer = L"p msedge.exe";

    status = DeviceIoControl(device, IOCTL_HIEU, &in_buffer[0], in_buffer.size() * sizeof(WCHAR), out_buffer, sizeof(out_buffer), &bytes_returned, (LPOVERLAPPED)NULL);

    in_buffer = L"f hide.txt";

    status = DeviceIoControl(device, IOCTL_HIEU, &in_buffer[0], in_buffer.size() * sizeof(WCHAR), out_buffer, sizeof(out_buffer), &bytes_returned, (LPOVERLAPPED)NULL);

    CloseHandle(device);
}