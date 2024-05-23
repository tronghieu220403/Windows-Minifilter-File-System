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

		process::DrvRegister();
		//protect_file::DrvRegister();
		hide_proc::DrvRegister();

		ioctl::DrvRegister(driver_object, registry_path);

		return;

	}


	void DrvUnload(PDRIVER_OBJECT driver_object)
	{
		DebugMessage("DriverUnloadRegistered");

		delete kDrvFuncVector;
		
		process::DrvUnload();
		//protect_file::DrvUnload();
		hide_proc::DrvUnload();

		ioctl::DrvUnload(driver_object);

		return;
	}


	void FltRegister()
	{
		DebugMessage("MiniFilterRegister");

		kFltFuncVector = new Vector<IrpMjFunc>();
		
		protect_file::FltRegister();

		hide_file::FltRegister();

		return;
	}

	void FltUnload()
	{
		delete kFltFuncVector;

		protect_file::FltUnload();

		hide_file::FltUnload();

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