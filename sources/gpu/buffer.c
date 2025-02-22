#include <kore3/gpu/buffer.h>

#ifdef KORE_DIRECT3D12
#include <kope/direct3d12/buffer_functions.h>
#endif

#ifdef KORE_METAL
#include <kope/metal/buffer_functions.h>
#endif

#ifdef KORE_VULKAN
#include <kope/vulkan/buffer_functions.h>
#endif

void kore_gpu_buffer_set_name(kore_gpu_buffer *buffer, const char *name) {
	KORE_GPU_CALL2(buffer_set_name, buffer, name);
}

void kore_gpu_buffer_destroy(kore_gpu_buffer *buffer) {
	KORE_GPU_CALL1(buffer_destroy, buffer);
}

void *kore_gpu_buffer_try_to_lock_all(kore_gpu_buffer *buffer) {
	return KORE_GPU_CALL1(buffer_try_to_lock_all, buffer);
}

void *kore_gpu_buffer_lock_all(kore_gpu_buffer *buffer) {
	return KORE_GPU_CALL1(buffer_lock_all, buffer);
}

void *kore_gpu_buffer_try_to_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	return KORE_GPU_CALL3(buffer_try_to_lock, buffer, offset, size);
}

void *kore_gpu_buffer_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	return KORE_GPU_CALL3(buffer_lock, buffer, offset, size);
}

void kore_gpu_buffer_unlock(kore_gpu_buffer *buffer) {
	KORE_GPU_CALL1(buffer_unlock, buffer);
}
