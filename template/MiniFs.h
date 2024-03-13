#pragma once

/*++

Module Name:

    MiniFs.h

Abstract:

    This is the main module of the MiniFs miniFilter driver.

Environment:

    Kernel mode

--*/

#include <fltKernel.h>
#include <dontuse.h>

#include "register.h"
#include "debug.h"

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")


PFLT_FILTER kFilterHandle;

#define PTDBG_TRACE_ROUTINES            0x00000001
#define PTDBG_TRACE_OPERATION_STATUS    0x00000002

/*************************************************************************
    Prototypes
*************************************************************************/

EXTERN_C_START

DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT driver_object,
    _In_ PUNICODE_STRING registry_path
);

NTSTATUS 
DriverUnload(
    PDRIVER_OBJECT driver_object
);

NTSTATUS
MiniFsInstanceSetup(
    _In_ PCFLT_RELATED_OBJECTS flt_objects,
    _In_ FLT_INSTANCE_SETUP_FLAGS flags,
    _In_ DEVICE_TYPE volume_device_type,
    _In_ FLT_FILESYSTEM_TYPE volume_filesystem_type
);

VOID
MiniFsInstanceTeardownStart(
    _In_ PCFLT_RELATED_OBJECTS flt_objects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS flags
);

VOID
MiniFsInstanceTeardownComplete(
    _In_ PCFLT_RELATED_OBJECTS flt_objects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS flags
);

NTSTATUS
MiniFsUnload(
    _In_ FLT_FILTER_UNLOAD_FLAGS flags
);

NTSTATUS
MiniFsInstanceQueryTeardown(
    _In_ PCFLT_RELATED_OBJECTS flt_objects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS flags
);

FLT_PREOP_CALLBACK_STATUS
MiniFsPreOperation(
    _Inout_ PFLT_CALLBACK_DATA data,
    _In_ PCFLT_RELATED_OBJECTS flt_objects,
    _Flt_CompletionContext_Outptr_ PVOID* completion_context
);

FLT_POSTOP_CALLBACK_STATUS
MiniFsPostOperation(
    _Inout_ PFLT_CALLBACK_DATA data,
    _In_ PCFLT_RELATED_OBJECTS flt_objects,
    _In_opt_ PVOID completion_context,
    _In_ FLT_POST_OPERATION_FLAGS flags
);

EXTERN_C_END

//
//  Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, MiniFsUnload)
#pragma alloc_text(PAGE, MiniFsInstanceQueryTeardown)
#pragma alloc_text(PAGE, MiniFsInstanceSetup)
#pragma alloc_text(PAGE, MiniFsInstanceTeardownStart)
#pragma alloc_text(PAGE, MiniFsInstanceTeardownComplete)
#endif

//
//  operation registration
//

CONST FLT_OPERATION_REGISTRATION kCallbacks[] = {

    { IRP_MJ_CREATE,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_CREATE_NAMED_PIPE,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_CLOSE,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_READ,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_WRITE,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_QUERY_INFORMATION,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_SET_INFORMATION,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_QUERY_EA,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_SET_EA,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_FLUSH_BUFFERS,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_QUERY_VOLUME_INFORMATION,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_SET_VOLUME_INFORMATION,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_DIRECTORY_CONTROL,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_FILE_SYSTEM_CONTROL,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_DEVICE_CONTROL,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_INTERNAL_DEVICE_CONTROL,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_SHUTDOWN,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      NULL },                               //post operations not supported

    { IRP_MJ_LOCK_CONTROL,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_CLEANUP,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_CREATE_MAILSLOT,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_QUERY_SECURITY,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_SET_SECURITY,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_QUERY_QUOTA,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_SET_QUOTA,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_PNP,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_ACQUIRE_FOR_MOD_WRITE,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_RELEASE_FOR_MOD_WRITE,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_ACQUIRE_FOR_CC_FLUSH,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_RELEASE_FOR_CC_FLUSH,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_NETWORK_QUERY_OPEN,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_MDL_READ,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_MDL_READ_COMPLETE,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_PREPARE_MDL_WRITE,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_MDL_WRITE_COMPLETE,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_VOLUME_MOUNT,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_VOLUME_DISMOUNT,
      0,
      (PFLT_PRE_OPERATION_CALLBACK)&MiniFsPreOperation,
      (PFLT_POST_OPERATION_CALLBACK)&MiniFsPostOperation },

    { IRP_MJ_OPERATION_END }
};

//
//  This defines what we want to filter with FltMgr
//

CONST FLT_REGISTRATION FilterRegistration = {

    sizeof(FLT_REGISTRATION),         //  Size
    FLT_REGISTRATION_VERSION,           //  Version
    0,                                  //  flags

    NULL,                               //  Context
    kCallbacks,                          //  Operation callbacks

    (PFLT_FILTER_UNLOAD_CALLBACK)MiniFsUnload,                                  //  MiniFilterUnload

    (PFLT_INSTANCE_SETUP_CALLBACK)MiniFsInstanceSetup,                           //  InstanceSetup
    (PFLT_INSTANCE_QUERY_TEARDOWN_CALLBACK)MiniFsInstanceQueryTeardown,         //  InstanceQueryTeardown
    (PFLT_INSTANCE_TEARDOWN_CALLBACK)MiniFsInstanceTeardownStart,               //  InstanceTeardownStart
    (PFLT_INSTANCE_TEARDOWN_CALLBACK)MiniFsInstanceTeardownComplete,            //  InstanceTeardownComplete

    NULL,                               //  GenerateFileName
    NULL,                               //  GenerateDestinationFileName
    NULL                                //  NormalizeNameComponent

};

