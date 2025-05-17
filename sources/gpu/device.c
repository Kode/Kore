#include <kore3/gpu/device.h>

#if defined(KORE_DIRECT3D11)
#include <kore3/direct3d11/device_functions.h>
#elif defined(KORE_DIRECT3D12)
#include <kore3/direct3d12/device_functions.h>
#elif defined(KORE_METAL)
#include <kore3/metal/device_functions.h>
#elif defined(KORE_OPENGL)
#include <kore3/opengl/device_functions.h>
#elif defined(KORE_VULKAN)
#include <kore3/vulkan/device_functions.h>
#elif defined(KORE_WEBGPU)
#include <kore3/webgpu/device_functions.h>
#else
#error("Unknown GPU backend")
#endif

#include <assert.h>

void kore_gpu_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist) {
	KORE_GPU_CALL2(device_create, device, wishlist);
}

void kore_gpu_device_destroy(kore_gpu_device *device) {
	KORE_GPU_CALL1(device_destroy, device);
}

void kore_gpu_device_set_name(kore_gpu_device *device, const char *name) {
	KORE_GPU_CALL2(device_set_name, device, name);
}

void kore_gpu_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer) {
#ifdef KORE_GPU_VALIDATION
	buffer->usage_flags = parameters->usage_flags;
#endif
	KORE_GPU_CALL3(device_create_buffer, device, parameters, buffer);
}

void kore_gpu_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {
	KORE_GPU_CALL3(device_create_command_list, device, type, list);
}

void kore_gpu_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {
#ifdef KORE_GPU_VALIDATION
	if (kore_gpu_texture_format_is_depth(parameters->format)) {
		assert(parameters->dimension != KORE_GPU_TEXTURE_DIMENSION_3D);
	}
#endif

	texture->format = parameters->format;
	KORE_GPU_CALL3(device_create_texture, device, parameters, texture);
}

kore_gpu_texture *kore_gpu_device_get_framebuffer(kore_gpu_device *device) {
	kore_gpu_texture *texture = KORE_GPU_CALL1(device_get_framebuffer, device);
	texture->format = kore_gpu_device_framebuffer_format(device);
	return texture;
}

kore_gpu_texture_format kore_gpu_device_framebuffer_format(kore_gpu_device *device) {
	return KORE_GPU_CALL1(device_framebuffer_format, device);
}

void kore_gpu_device_create_query_set(kore_gpu_device *device, const kore_gpu_query_set_parameters *parameters, kore_gpu_query_set *query_set) {
	KORE_GPU_CALL3(device_create_query_set, device, parameters, query_set);
}

void kore_gpu_device_create_fence(kore_gpu_device *device, kore_gpu_fence *fence) {
	KORE_GPU_CALL2(device_create_fence, device, fence);
}

void kore_gpu_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {
	KORE_GPU_CALL2(device_execute_command_list, device, list);
}

void kore_gpu_device_create_sampler(kore_gpu_device *device, const kore_gpu_sampler_parameters *parameters, kore_gpu_sampler *sampler) {
	KORE_GPU_CALL3(device_create_sampler, device, parameters, sampler);
}

void kore_gpu_device_create_raytracing_volume(kore_gpu_device *device, kore_gpu_buffer *vertex_buffer, uint64_t vertex_count, kore_gpu_buffer *index_buffer,
                                              uint32_t index_count, kore_gpu_raytracing_volume *volume) {
	KORE_GPU_CALL6(device_create_raytracing_volume, device, vertex_buffer, vertex_count, index_buffer, index_count, volume);
}

void kore_gpu_device_create_raytracing_hierarchy(kore_gpu_device *device, kore_gpu_raytracing_volume **volumes, kore_matrix4x4 *volume_transforms,
                                                 uint32_t volumes_count, kore_gpu_raytracing_hierarchy *hierarchy) {
	KORE_GPU_CALL5(device_create_raytracing_hierarchy, device, volumes, volume_transforms, volumes_count, hierarchy);
}

void kore_gpu_device_wait_until_idle(kore_gpu_device *device) {
	KORE_GPU_CALL1(device_wait_until_idle, device);
}

uint32_t kore_gpu_device_align_texture_row_bytes(kore_gpu_device *device, uint32_t row_bytes) {
	return KORE_GPU_CALL2(device_align_texture_row_bytes, device, row_bytes);
}

void kore_gpu_device_signal(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {
	KORE_GPU_CALL4(device_signal, device, list_type, fence, value);
}

void kore_gpu_device_wait(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {
	KORE_GPU_CALL4(device_wait, device, list_type, fence, value);
}
