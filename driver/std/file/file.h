/*
Write a class in Windows Kernel Driver that have the following operation on a file: open, read, write, append, close. C++ std libraries is not allowed.
*/

#ifndef FILE_H
#define FILE_H

#include "../string/string.h"

#include <ntddk.h>

namespace file
{
    class File {
    private:
        String<WCHAR> file_path_;
        HANDLE file_handle_ = nullptr;
    public:
        File(const String<WCHAR>& file_name);
        size_t Read(PVOID buffer, size_t length);
        size_t Append(PVOID buffer, size_t length);
        size_t Size();

        String<WCHAR> GetPath() const { return file_path_; }
        ~File();

    protected:
        bool Open(const WCHAR* file_path);
        void Close();
    };
}

#endif // FILE_H
