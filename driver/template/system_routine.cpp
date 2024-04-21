#include "system_routine.h"

void GetSystemRoutineAddresses()
{
	PS_GET_PROCESS_IMAGE_FILE_NAME PsGetProcessImageFileName = nullptr;
	ZW_QUERY_INFORMATION_PROCESS ZwQueryInformationProcess = nullptr;

	PsGetProcessImageFileName = (PS_GET_PROCESS_IMAGE_FILE_NAME)MmGetSystemRoutineAddress(&PS_GET_PROCESS_IMAGE_FILE_NAME_UNISTR);

	ZwQueryInformationProcess = (ZW_QUERY_INFORMATION_PROCESS)MmGetSystemRoutineAddress(&ZW_QUERY_INFORMATION_PROCESS_UNISTR);
}
