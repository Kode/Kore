#include <kore3/webgpu/buffer_functions.h>

#include <kore3/gpu/buffer.h>

void kore_webgpu_buffer_set_name(kore_gpu_buffer *buffer, const char *name) {}

void kore_webgpu_buffer_destroy(kore_gpu_buffer *buffer) {}

void *kore_webgpu_buffer_try_to_lock_all(kore_gpu_buffer *buffer) {
	return NULL;
}

void *kore_webgpu_buffer_lock_all(kore_gpu_buffer *buffer) {
	return NULL;
}

void *kore_webgpu_buffer_try_to_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	return NULL;
}

void *kore_webgpu_buffer_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	return NULL;
}

void kore_webgpu_buffer_unlock(kore_gpu_buffer *buffer) {}
