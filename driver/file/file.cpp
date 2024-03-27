#include "file.h"

namespace file
{
    File::File(const String<WCHAR>& file_name):
        file_path_(file_name)
    {
        Open(file_path_.Data());
    }
    bool File::Open(const WCHAR* file_path) {

        if (KeGetCurrentIrql() != PASSIVE_LEVEL)
        {
            return false;
        }

        UNICODE_STRING unicodeString;
        OBJECT_ATTRIBUTES objectAttributes;
        IO_STATUS_BLOCK io_status_block;

        RtlInitUnicodeString(&unicodeString, file_path);
        InitializeObjectAttributes(&objectAttributes, &unicodeString, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

        NTSTATUS status = ZwCreateFile(&file_handle_, FILE_GENERIC_READ | FILE_GENERIC_WRITE, &objectAttributes, &io_status_block, NULL, FILE_ATTRIBUTE_NORMAL, 0, FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
        return NT_SUCCESS(status);
    }

    bool File::Read(PVOID buffer, size_t length, size_t* _Out_ bytes_read) {
        if (KeGetCurrentIrql() != PASSIVE_LEVEL)
        {
            return false;
        }
        IO_STATUS_BLOCK io_status_block;

        NTSTATUS status = ZwReadFile(file_handle_, NULL, NULL, NULL, &io_status_block, buffer, (ULONG)length, NULL, NULL);
        *bytes_read = io_status_block.Information;
        return NT_SUCCESS(status);
    }

    bool File::Append(PVOID buffer, size_t length, size_t* bytes_appended) {
        if (KeGetCurrentIrql() != PASSIVE_LEVEL)
        {
            return false;
        }
        IO_STATUS_BLOCK io_status_block;
        NTSTATUS status;
        status = ZwWriteFile(file_handle_, NULL, NULL, NULL, &io_status_block, buffer, (ULONG)length, NULL, NULL);
        if (!NT_SUCCESS(status)) {
            return false;
        };

        *bytes_appended = io_status_block.Information;
        return NT_SUCCESS(status);
    }

    size_t File::Size()
    {
        if (KeGetCurrentIrql() != PASSIVE_LEVEL)
		{
			return 0;
		}
        IO_STATUS_BLOCK io_status_block;
        FILE_STANDARD_INFORMATION file_info;

        NTSTATUS status = ZwQueryInformationFile(file_handle_, &io_status_block, &file_info, sizeof(file_info), FileStandardInformation);
        if (!NT_SUCCESS(status)) {
			return 0;
		}
        return file_info.EndOfFile.QuadPart;
    }

    void File::Close() {
        if (KeGetCurrentIrql() != PASSIVE_LEVEL)
        {
            return;
        }
        ZwClose(file_handle_);
        file_handle_ = nullptr;
    }

    File::~File()
    {
        Close();
    }
}