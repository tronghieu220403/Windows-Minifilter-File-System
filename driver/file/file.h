/*
Write a class in Windows Kernel Driver that have the following operation on a file: open, read, write, append, close. C++ std libraries is not allowed.
*/

#ifndef FILE_H
#define FILE_H

#include "std/string/string.h"

#include <ntddk.h>

namespace file
{
    class File {
    private:
        String<WCHAR> file_path_;
        HANDLE file_handle_ = nullptr;
    public:
        File(const String<WCHAR>& file_name);
        bool Read(PVOID buffer, size_t length, size_t* _Out_ bytes_read);
        bool Append(PVOID buffer, size_t length, size_t* _Out_ bytes_appended);
        size_t Size();
        ~File();

    protected:
        bool Open(const WCHAR* file_path);
        void Close();
    };
}

#endif // FILE_H
