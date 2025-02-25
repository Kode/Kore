#include <kore3/webgpu/buffer_functions.h>

#include <kore3/gpu/buffer.h>

void kore_webgpu_buffer_set_name(kore_gpu_buffer *buffer, const char *name) {}

void kore_webgpu_buffer_destroy(kore_gpu_buffer *buffer) {}

void *kore_webgpu_buffer_try_to_lock_all(kore_gpu_buffer *buffer) {
	if (buffer->webgpu.has_copy_buffer) {
		return wgpuBufferGetMappedRange(buffer->webgpu.copy_buffer, 0, buffer->webgpu.size);
	}
	else {
		return wgpuBufferGetMappedRange(buffer->webgpu.buffer, 0, buffer->webgpu.size);
	}
}

void *kore_webgpu_buffer_lock_all(kore_gpu_buffer *buffer) {
	if (buffer->webgpu.has_copy_buffer) {
		return wgpuBufferGetMappedRange(buffer->webgpu.copy_buffer, 0, buffer->webgpu.size);
	}
	else {
		return wgpuBufferGetMappedRange(buffer->webgpu.buffer, 0, buffer->webgpu.size);
	}
}

void *kore_webgpu_buffer_try_to_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	if (buffer->webgpu.has_copy_buffer) {
		return wgpuBufferGetMappedRange(buffer->webgpu.copy_buffer, offset, size);
	}
	else {
		return wgpuBufferGetMappedRange(buffer->webgpu.buffer, offset, size);
	}
}

void *kore_webgpu_buffer_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	if (buffer->webgpu.has_copy_buffer) {
		return wgpuBufferGetMappedRange(buffer->webgpu.copy_buffer, offset, size);
	}
	else {
		return wgpuBufferGetMappedRange(buffer->webgpu.buffer, offset, size);
	}
}

void kore_webgpu_buffer_unlock(kore_gpu_buffer *buffer) {
	wgpuBufferUnmap(buffer->webgpu.buffer);
}
