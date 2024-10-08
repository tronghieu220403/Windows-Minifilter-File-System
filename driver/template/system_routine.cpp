#include "system_routine.h"

void GetSystemRoutineAddresses()
{
	UNICODE_STRING PS_GET_PROCESS_IMAGE_FILE_NAME_UNISTR = RTL_CONSTANT_STRING(L"PsGetProcessImageFileName");

	UNICODE_STRING ZW_QUERY_INFORMATION_PROCESS_UNISTR = RTL_CONSTANT_STRING(L"ZwQueryInformationProcess");

	PsGetProcessImageFileName = (PS_GET_PROCESS_IMAGE_FILE_NAME)MmGetSystemRoutineAddress(&PS_GET_PROCESS_IMAGE_FILE_NAME_UNISTR);

	ZwQueryInformationProcess = (ZW_QUERY_INFORMATION_PROCESS)MmGetSystemRoutineAddress(&ZW_QUERY_INFORMATION_PROCESS_UNISTR);
}
