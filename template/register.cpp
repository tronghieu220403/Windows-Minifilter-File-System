#include "register.h"

namespace reg
{
	Vector<IrpMjFunc>* kFltFuncVector = nullptr;

	Vector<void*>* kDriverFuncVector = nullptr;

	void DrvRegister(
		_In_ PDRIVER_OBJECT driver_object,
		_In_ PUNICODE_STRING registry_path
	)
	{
		DebugMessage("DriverRegister");

		kDriverFuncVector = new Vector<void*>();

		anti_delete::DrvRegister();

		// TODO
	}


	void DrvUnloadRegister(PDRIVER_OBJECT driver_object)
	{
		DebugMessage("DriverUnloadRegistered");

		delete kDriverFuncVector;
		anti_delete::DrvUnload();
		return;
	}


	void FltRegister()
	{
		DebugMessage("MiniFilterRegister");

		kFltFuncVector = new Vector<IrpMjFunc>();
		anti_delete::FltRegister();

		// TODO: register minifilter callback function here.
	}

	void FltUnload()
	{
		delete kFltFuncVector;
		anti_delete::FltUnload();
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

}