#pragma once
typedef unsigned long long size_t;
typedef unsigned long ULONG;
typedef unsigned char BYTE;
typedef char CHAR;
typedef short WCHAR;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned long long QWORD;
typedef unsigned short USHORT;
typedef long long ptrdiff_t;

#pragma warning(disable:4100)

#define max(X, Y) (((X) > (Y)) ? (X) : (Y))
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

#define INT_MAX 2147483647

template <typename T>
struct Less {
public:
    inline bool operator()(const T& lhs, const T& rhs) const {
        return lhs < rhs;
    }
};

template <typename T>
struct Greater {
public:
    inline bool operator()(const T& lhs, const T& rhs) const {
        return lhs > rhs;
    }
};
