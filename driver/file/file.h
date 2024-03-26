/*
Write a class in Windows Kernel Driver that have the following operation on a file: open, read, write, append, close. C++ std libraries is not allowed.
*/

#ifndef FILE_H
#define FILE_H

#include <ntddk.h>

class File {
private:
    HANDLE fileHandle;

public:
    File() : fileHandle(NULL) {}

    bool Open(const WCHAR* filePath) {
        UNICODE_STRING unicodeString;
        OBJECT_ATTRIBUTES objectAttributes;
        IO_STATUS_BLOCK ioStatusBlock;

        RtlInitUnicodeString(&unicodeString, filePath);
        InitializeObjectAttributes(&objectAttributes, &unicodeString, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

        NTSTATUS status = ZwCreateFile(&fileHandle, FILE_GENERIC_READ | FILE_GENERIC_WRITE, &objectAttributes, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, 0, FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
        return NT_SUCCESS(status);
    }

    bool Read(PVOID buffer, ULONG length, ULONG* bytesRead) {
        IO_STATUS_BLOCK ioStatusBlock;

        NTSTATUS status = ZwReadFile(fileHandle, NULL, NULL, NULL, &ioStatusBlock, buffer, length, NULL, NULL);
        *bytesRead = ioStatusBlock.Information;
        return NT_SUCCESS(status);
    }

    bool Write(PVOID buffer, ULONG length, ULONG* bytesWritten) {
        IO_STATUS_BLOCK ioStatusBlock;

        NTSTATUS status = ZwWriteFile(fileHandle, NULL, NULL, NULL, &ioStatusBlock, buffer, length, NULL, NULL);
        *bytesWritten = ioStatusBlock.Information;
        return NT_SUCCESS(status);
    }

    bool Append(PVOID buffer, ULONG length, ULONG* bytesAppended) {
        IO_STATUS_BLOCK ioStatusBlock;
        FILE_STANDARD_INFORMATION fileInfo;

        NTSTATUS status = ZwQueryInformationFile(fileHandle, &ioStatusBlock, &fileInfo, sizeof(fileInfo), FileStandardInformation);
        if (!NT_SUCCESS(status)) {
            return false;
        }

        LARGE_INTEGER offset;
        offset.QuadPart = fileInfo.EndOfFile.QuadPart;

        status = ZwWriteFile(fileHandle, NULL, NULL, NULL, &ioStatusBlock, buffer, length, &offset, NULL);
        *bytesAppended = ioStatusBlock.Information;
        return NT_SUCCESS(status);
    }

    void Close() {
        ZwClose(fileHandle);
        fileHandle = NULL;
    }
};

#endif // FILE_H
