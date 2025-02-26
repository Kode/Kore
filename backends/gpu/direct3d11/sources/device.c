#include "device_functions.h"

#include "d3d11unit.h"

#include <kore3/gpu/device.h>
#include <kore3/util/align.h>

#include <kore3/log.h>
#include <kore3/window.h>

#include <assert.h>

void kore_d3d11_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist) {}

void kore_d3d11_device_destroy(kore_gpu_device *device) {}

void kore_d3d11_device_set_name(kore_gpu_device *device, const char *name) {}

void kore_d3d11_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer) {}

void kore_d3d11_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {}

void kore_d3d11_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {}

kore_gpu_texture *kore_d3d11_device_get_framebuffer(kore_gpu_device *device) {
	return NULL;
}

void kore_d3d11_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {}

void kore_d3d11_device_wait_until_idle(kore_gpu_device *device) {}

void kore_d3d11_device_create_descriptor_set(kore_gpu_device *device, uint32_t descriptor_count, uint32_t dynamic_descriptor_count,
                                             uint32_t bindless_descriptor_count, uint32_t sampler_count, kore_d3d11_descriptor_set *set) {}

void kore_d3d11_device_create_sampler(kore_gpu_device *device, const kore_gpu_sampler_parameters *parameters, kore_gpu_sampler *sampler) {}

void kore_d3d11_device_create_raytracing_volume(kore_gpu_device *device, kore_gpu_buffer *vertex_buffer, uint64_t vertex_count, kore_gpu_buffer *index_buffer,
                                                uint32_t index_count, kore_gpu_raytracing_volume *volume) {}

void kore_d3d11_device_create_raytracing_hierarchy(kore_gpu_device *device, kore_gpu_raytracing_volume **volumes, kore_matrix4x4_t *volume_transforms,
                                                   uint32_t volumes_count, kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_d3d11_device_create_query_set(kore_gpu_device *device, const kore_gpu_query_set_parameters *parameters, kore_gpu_query_set *query_set) {}

uint32_t kore_d3d11_device_align_texture_row_bytes(kore_gpu_device *device, uint32_t row_bytes) {
	return 0;
}

void kore_d3d11_device_create_fence(kore_gpu_device *device, kore_gpu_fence *fence) {}

void kore_d3d11_device_signal(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}

void kore_d3d11_device_wait(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}
