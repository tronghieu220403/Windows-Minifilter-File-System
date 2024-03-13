#include "register.h"

Vector<IrpMjPreFunction>* kPreFuncVector = nullptr;

Vector<IrpMjPostFunction>* kPostFuncVector = nullptr;

Vector<void*>* kDriverFuncVector = nullptr;

void DriverRegister(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	DebugMessage("DriverRegister");
	kPreFuncVector = new Vector<IrpMjPreFunction>();
	kPostFuncVector = new Vector<IrpMjPostFunction>();
	kDriverFuncVector = new Vector<void*>();

	// TODO
}

void MiniFilterRegister()
{
	DebugMessage("MiniFilterRegister");

	// TODO: register minifilter callback function here.
}

void DriverUnloadRegistered(PDRIVER_OBJECT driver_object)
{
	DebugMessage("DriverUnloadRegistered");
	return;
}

Context* AllocCompletionContext()
{
	// DebugMessage("AllocCompletionContext");

	Context* context = new Context();
	context->status = new Vector<FLT_PREOP_CALLBACK_STATUS>(kPreFuncVector->Size());

	return context;

}

void DeallocCompletionContext(Context *context)
{
	// DebugMessage("DeallocCompletionContext");

	delete context->status;

	delete context;
}
