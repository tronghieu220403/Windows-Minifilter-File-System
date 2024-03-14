#include "register.h"

Vector<IrpMjFunc>* kFltFuncVector = nullptr;

Vector<void*>* kDriverFuncVector = nullptr;

void DriverRegister(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	DebugMessage("DriverRegister");
	kFltFuncVector = new Vector<IrpMjFunc>();
	kDriverFuncVector = new Vector<void*>();

	// TODO
}

void MiniFilterRegister()
{
	DebugMessage("MiniFilterRegister");

	anti_delete::FltRegister();

	// TODO: register minifilter callback function here.
}

void DriverUnloadRegister(PDRIVER_OBJECT driver_object)
{
	DebugMessage("DriverUnloadRegistered");
	return;
}

Context* AllocCompletionContext()
{
	// DebugMessage("AllocCompletionContext");

	Context* context = new Context();
	context->status = new Vector<FLT_PREOP_CALLBACK_STATUS>(kFltFuncVector->Size());

	return context;

}

void DeallocCompletionContext(Context *context)
{
	// DebugMessage("DeallocCompletionContext");

	delete context->status;

	delete context;
}
