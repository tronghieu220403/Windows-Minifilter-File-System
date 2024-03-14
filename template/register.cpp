#include "register.h"

namespace reg
{
	Vector<IrpMjFunc>* kFltFuncVector = nullptr;

	Vector<void*>* kDrvFuncVector = nullptr;

	void DrvRegister(
		_In_ PDRIVER_OBJECT driver_object,
		_In_ PUNICODE_STRING registry_path
	)
	{
		DebugMessage("DriverRegister");

		kDrvFuncVector = new Vector<void*>();

		anti_delete::DrvRegister();
		hide::DrvRegister();
		return;
	}


	void DrvUnload(PDRIVER_OBJECT driver_object)
	{
		DebugMessage("DriverUnloadRegistered");

		delete kDrvFuncVector;
		
		anti_delete::DrvUnload();
		hide::DrvUnload();
		return;
	}


	void FltRegister()
	{
		DebugMessage("MiniFilterRegister");

		kFltFuncVector = new Vector<IrpMjFunc>();
		
		anti_delete::FltRegister();
		hide::FltRegister();

		return;
	}

	void FltUnload()
	{
		delete kFltFuncVector;
		anti_delete::FltUnload();
		hide::FltUnload();
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

}