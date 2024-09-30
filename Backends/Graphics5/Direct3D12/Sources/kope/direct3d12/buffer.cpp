#include "buffer_functions.h"

#include <kope/graphics5/buffer.h>

#include <kinc/backend/SystemMicrosoft.h>

void kope_d3d12_buffer_set_name(kope_g5_buffer *buffer, const char *name) {
	wchar_t wstr[1024];
	kinc_microsoft_convert_string(wstr, name, 1024);
	buffer->d3d12.resource->SetName(wstr);
}

void kope_d3d12_buffer_destroy(kope_g5_buffer *buffer) {
	buffer->d3d12.resource->Release();
}

void *kope_d3d12_buffer_try_to_lock(kope_g5_buffer *buffer) {
	if (check_for_fence(buffer->d3d12.device->d3d12.execution_fence, buffer->d3d12.latest_execution_index)) {
		void *data = NULL;
		buffer->d3d12.resource->Map(0, NULL, &data);
		return data;
	}
	else {
		return NULL;
	}
}

void *kope_d3d12_buffer_lock(kope_g5_buffer *buffer) {
	wait_for_fence(buffer->d3d12.device, buffer->d3d12.device->d3d12.execution_fence, buffer->d3d12.device->d3d12.execution_event,
	               buffer->d3d12.latest_execution_index);

	void *data = NULL;
	buffer->d3d12.resource->Map(0, NULL, &data);
	return data;
}

void kope_d3d12_buffer_unlock(kope_g5_buffer *buffer) {
	buffer->d3d12.resource->Unmap(0, NULL); // doesn't actually do anything in D3D12, just helps with debugging
}
