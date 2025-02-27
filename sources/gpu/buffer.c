#include <kore3/gpu/buffer.h>

#if defined(KORE_DIRECT3D11)
#include <kore3/direct3d11/buffer_functions.h>
#elif defined(KORE_DIRECT3D12)
#include <kore3/direct3d12/buffer_functions.h>
#elif defined(KORE_METAL)
#include <kore3/metal/buffer_functions.h>
#elif defined(KORE_OPENGL)
#include <kore3/opengl/buffer_functions.h>
#elif defined(KORE_VULKAN)
#include <kore3/vulkan/buffer_functions.h>
#elif defined(KORE_WEBGPU)
#include <kore3/webgpu/buffer_functions.h>
#else
#error("Unknown GPU backend")
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
