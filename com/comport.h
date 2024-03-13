#pragma once

#include <fltKernel.h>
#include <dontuse.h>

#include "Windows-Minifilter-Template/std/vector/vector.h"

namespace com
{
	class ComPort
	{
	private:
		inline static PFLT_FILTER p_filter_handle_ = { 0 };
		inline static PSECURITY_DESCRIPTOR sec_des_ = { 0 };
		inline static PFLT_PORT server_port_ = { 0 };
		inline static OBJECT_ATTRIBUTES obj_attr_ = { 0 };
		inline static PFLT_PORT client_port_ = { 0 };

	public:
		static NTSTATUS Create();
		static NTSTATUS Send(PVOID sender_buffer, ULONG sender_buffer_length);
		static NTSTATUS Close();

		static NTSTATUS ConnectHandler(
			PFLT_PORT client_port,
			PVOID server_port_cookie,
			PVOID connection_context,
			ULONG size_of_context,
			PVOID* connection_port_cookie
		);

		static VOID DisonnectHandler(
			PVOID connection_cookie
		);

		static NTSTATUS SendRecvHandler(
			PVOID port_cookie,
			PVOID input_buffer,
			ULONG input_buffer_length,
			PVOID output_buffer,
			ULONG output_buffer_length,
			PULONG return_output_buffer_length
		);

		static void SetPfltFilter(PFLT_FILTER p_filter_handle);
		static PFLT_FILTER GetPfltFilter();

	};
}