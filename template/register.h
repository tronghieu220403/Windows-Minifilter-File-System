#pragma once

#include <Ntifs.h>
#include <fltKernel.h>

#include "../std/vector/vector.h"
#include "../template/debug.h"

struct IrpMjPreFunction
{
	size_t irp_mj_function_code = 0;
	PFLT_PRE_OPERATION_CALLBACK func = nullptr;
};

struct IrpMjPostFunction
{
	size_t irp_mj_function_code = 0;
	PFLT_POST_OPERATION_CALLBACK func = nullptr;

};

struct Context
{
	Vector<FLT_PREOP_CALLBACK_STATUS>* status = nullptr;
};

extern Vector<IrpMjPreFunction>* kPreFuncVector;

extern Vector<IrpMjPostFunction>* kPostFuncVector;

extern Vector<void*>* kDriverFuncVector;

void DriverRegister(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
);

void MiniFilterRegister();

void DriverUnloadRegistered(PDRIVER_OBJECT driver_object);

Context* AllocCompletionContext();

void DeallocCompletionContext(Context*);
