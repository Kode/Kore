#include <kore3/metal/device_functions.h>

#include "metalunit.h"

#include <kore3/gpu/device.h>
#include <kore3/util/align.h>

#include <kinc/log.h>
#include <kinc/window.h>

#include <assert.h>

void kore_metal_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist) {
	id<MTLDevice> metal_device = MTLCreateSystemDefaultDevice();
	getMetalLayer().device = metal_device;
	device->metal.device = (__bridge_retained void *)metal_device;
	device->metal.library = (__bridge_retained void *)[metal_device newDefaultLibrary];
}

void kore_metal_device_destroy(kore_gpu_device *device) {}

void kore_metal_device_set_name(kore_gpu_device *device, const char *name) {}

void kore_metal_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer) {
	id<MTLDevice> metal_device = (__bridge id<MTLDevice>)device->metal.device;
	MTLResourceOptions options = MTLResourceCPUCacheModeWriteCombined;
#ifdef KINC_APPLE_SOC
	options |= MTLResourceStorageModeShared;
#else
	if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_CPU_WRITE) != 0) {
		options |= MTLResourceStorageModeManaged;
	}
	else {
		options |= MTLResourceStorageModeShared;
	}
#endif
	id<MTLBuffer> metal_buffer = [metal_device newBufferWithLength:parameters->size options:options];
	buffer->metal.buffer = (__bridge_retained void *)metal_buffer;
}

void kore_metal_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {
	id<MTLDevice> metal_device = (__bridge id<MTLDevice>)device->metal.device;
	id<MTLCommandQueue> command_queue = [metal_device newCommandQueue];
	list->metal.command_queue = (__bridge_retained void *)command_queue;
	list->metal.command_buffer = (__bridge_retained void *)[command_queue commandBuffer];
}

void kore_metal_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {}

static kore_gpu_texture framebuffer;

kore_gpu_texture *kore_metal_device_get_framebuffer(kore_gpu_device *device) {
	CAMetalLayer *metal_layer = getMetalLayer();
	id<CAMetalDrawable> drawable = [metal_layer nextDrawable];
	framebuffer.metal.texture = (__bridge_retained void *)drawable.texture;
	return &framebuffer;
}

kore_gpu_texture_format kore_metal_device_framebuffer_format(kore_gpu_device *device) {
	return KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM;
}

void kore_metal_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {
	id<MTLCommandBuffer> command_buffer = (__bridge id<MTLCommandBuffer>)list->metal.command_buffer;
	[command_buffer commit];

	id<MTLCommandQueue> command_queue = (__bridge id<MTLCommandQueue>)list->metal.command_queue;
	command_buffer = [command_queue commandBuffer];
	list->metal.command_buffer = (__bridge_retained void *)[command_queue commandBuffer];
}

void kore_metal_device_wait_until_idle(kore_gpu_device *device) {}

void kore_metal_device_create_descriptor_set(kore_gpu_device *device, uint32_t descriptor_count, uint32_t dynamic_descriptor_count,
                                             uint32_t bindless_descriptor_count, uint32_t sampler_count, kore_metal_descriptor_set *set) {}

void kore_metal_device_create_sampler(kore_gpu_device *device, const kore_gpu_sampler_parameters *parameters, kore_gpu_sampler *sampler) {}

void kore_metal_device_create_raytracing_volume(kore_gpu_device *device, kore_gpu_buffer *vertex_buffer, uint64_t vertex_count, kore_gpu_buffer *index_buffer,
                                                uint32_t index_count, kore_gpu_raytracing_volume *volume) {}

void kore_metal_device_create_raytracing_hierarchy(kore_gpu_device *device, kore_gpu_raytracing_volume **volumes, kinc_matrix4x4_t *volume_transforms,
                                                   uint32_t volumes_count, kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_metal_device_create_query_set(kore_gpu_device *device, const kore_gpu_query_set_parameters *parameters, kore_gpu_query_set *query_set) {}

uint32_t kore_metal_device_align_texture_row_bytes(kore_gpu_device *device, uint32_t row_bytes) {
	return 0;
}

void kore_metal_device_create_fence(kore_gpu_device *device, kore_gpu_fence *fence) {}

void kore_metal_device_signal(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}

void kore_metal_device_wait(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}
