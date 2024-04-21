#pragma once

#include "debug.h"

#include <ntddk.h>
typedef PCHAR(*PS_GET_PROCESS_IMAGE_FILE_NAME) (
	PEPROCESS Process
	);

typedef NTSTATUS (*ZW_QUERY_INFORMATION_PROCESS) (
	HANDLE           ProcessHandle,
	PROCESSINFOCLASS ProcessInformationClass,
	PVOID            ProcessInformation,
	ULONG            ProcessInformationLength,
	PULONG           ReturnLength
	);

inline PS_GET_PROCESS_IMAGE_FILE_NAME PsGetProcessImageFileName;
inline ZW_QUERY_INFORMATION_PROCESS ZwQueryInformationProcess;

void GetSystemRoutineAddresses();
