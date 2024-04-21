#pragma once

#include "debug.h"

#include <ntddk.h>

UNICODE_STRING PS_GET_PROCESS_IMAGE_FILE_NAME_UNISTR = RTL_CONSTANT_STRING(L"PsGetProcessImageFileName");

UNICODE_STRING ZW_QUERY_INFORMATION_PROCESS_UNISTR = RTL_CONSTANT_STRING(L"ZwQueryInformationProcess");

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

extern PS_GET_PROCESS_IMAGE_FILE_NAME PsGetProcessImageFileName;
extern ZW_QUERY_INFORMATION_PROCESS ZwQueryInformationProcess;

void GetSystemRoutineAddresses();
