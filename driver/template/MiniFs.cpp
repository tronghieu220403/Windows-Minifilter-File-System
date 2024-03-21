#include "../template/MiniFs.h"

NTSTATUS
MiniFsInstanceSetup (
    _In_ PCFLT_RELATED_OBJECTS flt_objects,
    _In_ FLT_INSTANCE_SETUP_FLAGS flags,
    _In_ DEVICE_TYPE volume_device_type,
    _In_ FLT_FILESYSTEM_TYPE volume_filesystem_type
    )
/*++

Routine Description:

    This routine is called whenever a new instance is created on a volume. This
    gives us a chance to decide if we need to attach to this volume or not.

    If this routine is not defined in the registration structure, automatic
    instances are always created.

Arguments:

    flt_objects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    flags - flags describing the reason for this attach request.

Return Value:

    STATUS_SUCCESS - attach
    STATUS_FLT_DO_NOT_ATTACH - do not attach

--*/
{
    UNREFERENCED_PARAMETER( flt_objects );
    UNREFERENCED_PARAMETER( flags );
    UNREFERENCED_PARAMETER( volume_device_type );
    UNREFERENCED_PARAMETER( volume_filesystem_type );

    PAGED_CODE();

    return STATUS_SUCCESS;
}


NTSTATUS
MiniFsInstanceQueryTeardown (
    _In_ PCFLT_RELATED_OBJECTS flt_objects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS flags
    )
/*++

Routine Description:

    This is called when an instance is being manually deleted by a
    call to FltDetachVolume or FilterDetach thereby giving us a
    chance to fail that detach request.

    If this routine is not defined in the registration structure, explicit
    detach requests via FltDetachVolume or FilterDetach will always be
    failed.

Arguments:

    flt_objects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    flags - Indicating where this detach request came from.

Return Value:

    Returns the status of this operation.

--*/
{
    UNREFERENCED_PARAMETER( flt_objects );
    UNREFERENCED_PARAMETER( flags );

    PAGED_CODE();

    return STATUS_SUCCESS;
}


VOID
MiniFsInstanceTeardownStart (
    _In_ PCFLT_RELATED_OBJECTS flt_objects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS flags
    )
/*++

Routine Description:

    This routine is called at the start of instance teardown.

Arguments:

    flt_objects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    flags - Reason why this instance is being deleted.

Return Value:

    None.

--*/
{
    UNREFERENCED_PARAMETER( flt_objects );
    UNREFERENCED_PARAMETER( flags );

    PAGED_CODE();

}


VOID
MiniFsInstanceTeardownComplete (
    _In_ PCFLT_RELATED_OBJECTS flt_objects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS flags
    )
/*++

Routine Description:

    This routine is called at the end of instance teardown.

Arguments:

    flt_objects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    flags - Reason why this instance is being deleted.

Return Value:

    None.

--*/
{
    UNREFERENCED_PARAMETER( flt_objects );
    UNREFERENCED_PARAMETER( flags );

    PAGED_CODE();

}


/*************************************************************************
    MiniFilter initialization and unload routines.
*************************************************************************/

NTSTATUS
DriverEntry (
    _In_ PDRIVER_OBJECT driver_object,
    _In_ PUNICODE_STRING registry_path
    )
/*++

Routine Description:

    This is the initialization routine for this miniFilter driver.  This
    registers with FltMgr and initializes all global data structures.

Arguments:

    driver_object - Pointer to driver object created by the system to
        represent this driver.

    registry_path - Unicode string identifying where the parameters for this
        driver are located in the registry.

Return Value:

    Routine can return non success error codes.

--*/
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER( registry_path );

    reg::DrvRegister(driver_object, registry_path);

    driver_object->DriverUnload = (PDRIVER_UNLOAD)DriverUnload;

    reg::FltRegister();

    //
    //  Register with FltMgr to tell it our callback routines
    //

    status = FltRegisterFilter( driver_object,
                                &FilterRegistration,
                                &kFilterHandle );

    FLT_ASSERT( NT_SUCCESS( status ) );

    if (NT_SUCCESS( status )) {

        //
        //  Start filtering i/o
        //

        status = FltStartFiltering( kFilterHandle );

        if (!NT_SUCCESS( status )) {

            FltUnregisterFilter( kFilterHandle );
        }
    }

    return status;
}

NTSTATUS 
DriverUnload(
    PDRIVER_OBJECT driver_object
)
{
    reg::DrvUnload(driver_object);
    return STATUS_SUCCESS;
}

NTSTATUS
MiniFsUnload (
    _In_ FLT_FILTER_UNLOAD_FLAGS flags
    )
/*++

Routine Description:

    This is the unload routine for this miniFilter driver. This is called
    when the minifilter is about to be unloaded. We can fail this unload
    request if this is not a mandatory unload indicated by the flags
    parameter.

Arguments:

    flags - Indicating if this is a mandatory unload.

Return Value:

    Returns STATUS_SUCCESS.

--*/
{
    UNREFERENCED_PARAMETER( flags );

    PAGED_CODE();

    reg::FltUnload();

    FltUnregisterFilter( kFilterHandle );

    DebugMessage("FilterUnloadRegistered");

    return STATUS_SUCCESS;
}


/*************************************************************************
    MiniFilter callback routines.
*************************************************************************/
FLT_PREOP_CALLBACK_STATUS
MiniFsPreOperation (
    _Inout_ PFLT_CALLBACK_DATA data,
    _In_ PCFLT_RELATED_OBJECTS flt_objects,
    _Flt_CompletionContext_Outptr_ PVOID *completion_context
    )
/*++

Routine Description:

    This routine is a pre-operation dispatch routine for this miniFilter.

    This is non-pageable because it could be called on the paging path

Arguments:

    data - Pointer to the filter callbackData that is passed to us.

    flt_objects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    completion_context - The context for the completion routine for this
        operation.

Return Value:

    The return value is the status of the operation.

--*/
{

    UNREFERENCED_PARAMETER( flt_objects );
    UNREFERENCED_PARAMETER( completion_context );

    
    reg::Context* p = nullptr;
    if ((*completion_context) == nullptr)
    {
        p = reg::AllocCompletionContext();
        (*completion_context) = (PVOID* )reg::AllocCompletionContext();
        p->status->Resize(reg::kFltFuncVector->Size());
    }
    else
    {
        p = (reg::Context*)(*completion_context);
    }

    if (p == nullptr)
    {
        return FLT_PREOP_SUCCESS_NO_CALLBACK;
    }

    for (int i = 0; i < reg::kFltFuncVector->Size(); i++)
    {
        if (data->Iopb->MajorFunction == (*reg::kFltFuncVector)[i].irp_mj_function_code &&
            (*reg::kFltFuncVector)[i].pre_func != nullptr)
        {
            FLT_PREOP_CALLBACK_STATUS status = (*reg::kFltFuncVector)[i].pre_func(data, flt_objects, completion_context);
            (*(p->status))[i] = status;
        }
    }

    if (data->Iopb->MajorFunction == IRP_MJ_SHUTDOWN)
    {
        DeallocCompletionContext(p);
        return FLT_PREOP_SUCCESS_NO_CALLBACK;
    }

    return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

FLT_POSTOP_CALLBACK_STATUS
MiniFsPostOperation (
    _Inout_ PFLT_CALLBACK_DATA data,
    _In_ PCFLT_RELATED_OBJECTS flt_objects,
    _In_opt_ PVOID completion_context,
    _In_ FLT_POST_OPERATION_FLAGS flags
    )
/*++

Routine Description:

    This routine is the post-operation completion routine for this
    miniFilter.

    This is non-pageable because it may be called at DPC level.

Arguments:

    data - Pointer to the filter callbackData that is passed to us.

    flt_objects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    completion_context - The completion context set in the pre-operation routine.

    flags - Denotes whether the completion is successful or is being drained.

Return Value:

    The return value is the status of the operation.

--*/
{
    UNREFERENCED_PARAMETER( data );
    UNREFERENCED_PARAMETER( flt_objects );
    UNREFERENCED_PARAMETER( completion_context );
    UNREFERENCED_PARAMETER( flags );

    
    reg::Context* p = (reg::Context*)completion_context;
    if (p == nullptr)
    {
        return FLT_POSTOP_FINISHED_PROCESSING;
    }

    for (int i = 0; i < (*reg::kFltFuncVector).Size(); i++)
    {
        if (data->Iopb->MajorFunction == (*reg::kFltFuncVector)[i].irp_mj_function_code &&
            (*reg::kFltFuncVector)[i].post_func != nullptr)
        {
            if ((*(p->status))[i] != FLT_PREOP_SUCCESS_NO_CALLBACK)
            {
                (*reg::kFltFuncVector)[i].post_func(data, flt_objects, completion_context, flags);
            }
        }
    }
    DeallocCompletionContext(p);
    return FLT_POSTOP_FINISHED_PROCESSING;
}

