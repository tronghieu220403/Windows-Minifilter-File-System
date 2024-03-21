#pragma once

#define HIEU_DEBUG

#ifdef HIEU_DEBUG

#define DebugMessage(x, ...) DbgPrintEx(0, 0, x, __VA_ARGS__)

#else 

#define DebugMessage(x, ...) 

#endif // DEBUG

#include <fltKernel.h>

#pragma warning( disable : 4083 4024 4047 4702 4189 4101 4100)

