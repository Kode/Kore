#include <kore3/metal/device_functions.h>

#include "metalunit.h"

#include <kore3/gpu/device.h>
#include <kore3/util/align.h>

#include <kore3/log.h>
#include <kore3/window.h>

#include <assert.h>

static void create_execution_fence(kore_gpu_device *device) {
	kore_metal_execution_fence *execution_fence = &device->metal.execution_fence;

	for (uint32_t fence_index = 0; fence_index < KORE_METAL_EXECUTION_FENCE_COUNT; ++fence_index) {
		execution_fence->commend_buffer_execution_indices[fence_index] = 0;
	}

	execution_fence->completed_index      = 0;
	execution_fence->next_execution_index = 1;
}

static uint64_t find_completed_execution(kore_gpu_device *device) {
	kore_metal_execution_fence *execution_fence = &device->metal.execution_fence;

	for (uint32_t fence_index = 0; fence_index < KORE_METAL_EXECUTION_FENCE_COUNT; ++fence_index) {
		if (execution_fence->commend_buffer_execution_indices[fence_index] != 0) {
			id<MTLCommandBuffer> command_buffer = (__bridge id<MTLCommandBuffer>)execution_fence->command_buffers[fence_index];
			if ([command_buffer status] == MTLCommandBufferStatusCompleted) {
				if (execution_fence->commend_buffer_execution_indices[fence_index] > execution_fence->completed_index) {
					execution_fence->completed_index = execution_fence->commend_buffer_execution_indices[fence_index];
				}

				CFRelease(execution_fence->command_buffers[fence_index]);
				execution_fence->command_buffers[fence_index] = NULL;

				execution_fence->commend_buffer_execution_indices[fence_index] = 0;
			}
		}
	}

	return execution_fence->completed_index;
}

static void wait_for_execution(kore_gpu_device *device, uint64_t index) {
	kore_metal_execution_fence *execution_fence = &device->metal.execution_fence;

	uint64_t completed = execution_fence->completed_index;

	if (completed >= index) {
		return;
	}

	completed = find_completed_execution(device);

	if (completed >= index) {
		return;
	}

	bool fence_found __attribute__((unused)) = false;

	for (uint32_t fence_index = 0; fence_index < KORE_METAL_EXECUTION_FENCE_COUNT; ++fence_index) {
		uint64_t value = execution_fence->commend_buffer_execution_indices[fence_index];

		if (value == index) {
			id<MTLCommandBuffer> command_buffer = (__bridge id<MTLCommandBuffer>)execution_fence->command_buffers[fence_index];
			[command_buffer waitUntilCompleted];

			CFRelease(execution_fence->command_buffers[fence_index]);
			execution_fence->command_buffers[fence_index] = NULL;

			execution_fence->commend_buffer_execution_indices[fence_index] = 0;

			execution_fence->completed_index = value;

			fence_found = true;

			break;
		}
	}

	assert(fence_found);
}

static void set_next_fence(kore_gpu_device *device, void *fence) {
	kore_metal_execution_fence *execution_fence = &device->metal.execution_fence;

	for (uint32_t fence_index = 0; fence_index < KORE_METAL_EXECUTION_FENCE_COUNT; ++fence_index) {
		uint64_t value = execution_fence->commend_buffer_execution_indices[fence_index];

		if (value == 0) {
			execution_fence->commend_buffer_execution_indices[fence_index] = execution_fence->next_execution_index;
			++execution_fence->next_execution_index;

			execution_fence->command_buffers[fence_index] = fence;
			return;
		}
	}

	uint64_t lowest_value       = UINT64_MAX;
	void    *lowest_fence       = NULL;
	uint32_t lowest_fence_index = UINT32_MAX;

	for (uint32_t fence_index = 0; fence_index < KORE_METAL_EXECUTION_FENCE_COUNT; ++fence_index) {
		uint64_t value = execution_fence->commend_buffer_execution_indices[fence_index];

		if (value < lowest_value) {
			lowest_fence       = execution_fence->command_buffers[fence_index];
			lowest_value       = value;
			lowest_fence_index = fence_index;
		}
	}

	id<MTLCommandBuffer> command_buffer = (__bridge id<MTLCommandBuffer>)lowest_fence;
	[command_buffer waitUntilCompleted];

	CFRelease(lowest_fence);
	execution_fence->command_buffers[lowest_fence_index] = fence;

	execution_fence->commend_buffer_execution_indices[lowest_fence_index] = execution_fence->next_execution_index;
	++execution_fence->next_execution_index;
}

void kore_metal_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist) {
	id<MTLDevice> metal_device = getMetalLayer().device;

	if (metal_device == nil) {
		metal_device = MTLCreateSystemDefaultDevice();
	}

	device->metal.device  = (__bridge_retained void *)metal_device;
	device->metal.library = (__bridge_retained void *)[metal_device newDefaultLibrary];

	create_execution_fence(device);
}

void kore_metal_device_destroy(kore_gpu_device *device) {}

void kore_metal_device_set_name(kore_gpu_device *device, const char *name) {}

void kore_metal_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer) {
	id<MTLDevice>      metal_device = (__bridge id<MTLDevice>)device->metal.device;
	MTLResourceOptions options      = MTLResourceCPUCacheModeWriteCombined;
#ifdef KORE_APPLE_SOC
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
	buffer->metal.buffer       = (__bridge_retained void *)metal_buffer;
	buffer->metal.host_visible =
	    (parameters->usage_flags & KORE_GPU_BUFFER_USAGE_CPU_WRITE) != 0 || (parameters->usage_flags & KORE_GPU_BUFFER_USAGE_CPU_READ) != 0;
	buffer->metal.size   = parameters->size;
	buffer->metal.device = device;
}

void kore_metal_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {
	id<MTLDevice>       metal_device         = (__bridge id<MTLDevice>)device->metal.device;
	id<MTLCommandQueue> command_queue        = [metal_device newCommandQueue];
	list->metal.command_queue                = (__bridge_retained void *)command_queue;
	list->metal.command_buffer               = (__bridge_retained void *)[command_queue commandBuffer];
	list->metal.render_command_encoder       = NULL;
	list->metal.compute_command_encoder      = NULL;
	list->metal.blit_command_encoder         = NULL;
	list->metal.queued_buffer_accesses_count = 0;
}

void kore_metal_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {
	MTLTextureDescriptor *descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:convert_format(parameters->format)
	                                                                                      width:parameters->width
	                                                                                     height:parameters->height
	                                                                                  mipmapped:NO];
	switch (parameters->dimension) {
	case KORE_GPU_TEXTURE_DIMENSION_1D:
		descriptor.textureType = parameters->depth_or_array_layers > 1 ? MTLTextureType1DArray : MTLTextureType1D;
		descriptor.width       = parameters->width;
		assert(parameters->height == 1);
		descriptor.height      = 1;
		descriptor.depth       = 1;
		descriptor.arrayLength = parameters->depth_or_array_layers;
		break;
	case KORE_GPU_TEXTURE_DIMENSION_2D:
		descriptor.width  = parameters->width;
		descriptor.height = parameters->height;
		descriptor.depth  = 1;

		if (parameters->depth_or_array_layers == 6) {
			descriptor.textureType = MTLTextureTypeCube;
			descriptor.arrayLength = 1;
			// TODO: Convert to array-texture when used as such in a view
		}
		else {
			descriptor.textureType = parameters->depth_or_array_layers > 1 ? MTLTextureType2DArray : MTLTextureType2D;
			descriptor.arrayLength = parameters->depth_or_array_layers;
		}
		break;
	case KORE_GPU_TEXTURE_DIMENSION_3D:
		descriptor.textureType = MTLTextureType3D;
		descriptor.width       = parameters->width;
		descriptor.height      = parameters->height;
		descriptor.depth       = parameters->depth_or_array_layers;
		descriptor.arrayLength = 1;
		break;
	}

	descriptor.pixelFormat      = convert_format(parameters->format);
	descriptor.mipmapLevelCount = parameters->mip_level_count;

	if ((parameters->usage & KORE_METAL_TEXTURE_USAGE_READ_WRITE) != 0) {
		descriptor.usage |= MTLTextureUsageShaderWrite | MTLTextureUsageShaderRead;
	}
	if ((parameters->usage & KORE_GPU_TEXTURE_USAGE_RENDER_ATTACHMENT) != 0) {
		descriptor.usage |= MTLTextureUsageRenderTarget;
	}

	id<MTLDevice> metal_device = (__bridge id<MTLDevice>)device->metal.device;
	texture->metal.texture     = (__bridge_retained void *)[metal_device newTextureWithDescriptor:descriptor];
	texture->metal.dimension   = parameters->dimension;

	texture->width  = parameters->width;
	texture->height = parameters->height;
}

static kore_gpu_texture framebuffer;

kore_gpu_texture *kore_metal_device_get_framebuffer(kore_gpu_device *device) {
	CAMetalLayer *metal_layer = getMetalLayer();

	if (drawable == nil) {
		drawable = [metal_layer nextDrawable];
	}

	framebuffer.metal.texture = (__bridge_retained void *)drawable.texture;

	framebuffer.width  = kore_window_width(0);
	framebuffer.height = kore_window_height(0);

	return &framebuffer;
}

kore_gpu_texture_format kore_metal_device_framebuffer_format(kore_gpu_device *device) {
	return KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM;
}

static void clean_buffer_accesses(kore_metal_buffer *buffer, uint64_t finished_execution_index) {
	kore_metal_buffer_range ranges[KORE_METAL_MAX_BUFFER_RANGES];
	uint32_t                ranges_count = 0;

	for (uint32_t range_index = 0; range_index < buffer->ranges_count; ++range_index) {
		if (buffer->ranges[range_index].execution_index > finished_execution_index) {
			ranges[ranges_count] = buffer->ranges[range_index];
			ranges_count += 1;
		}
	}

	memcpy(&buffer->ranges, &ranges, sizeof(ranges));
	buffer->ranges_count = ranges_count;
}

void kore_metal_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {
	kore_metal_command_list_end_compute_pass(list);
	kore_metal_command_list_end_blit_pass(list);

	uint64_t execution_index = device->metal.execution_fence.next_execution_index;

	for (uint32_t buffer_access_index = 0; buffer_access_index < list->metal.queued_buffer_accesses_count; ++buffer_access_index) {
		kore_metal_buffer_access access = list->metal.queued_buffer_accesses[buffer_access_index];
		kore_metal_buffer       *buffer = access.buffer;

		clean_buffer_accesses(buffer, find_completed_execution(device));

		assert(buffer->ranges_count < KORE_METAL_MAX_BUFFER_RANGES);
		buffer->ranges[buffer->ranges_count].execution_index = execution_index;
		buffer->ranges[buffer->ranges_count].offset          = access.offset;
		buffer->ranges[buffer->ranges_count].size            = access.size;
		buffer->ranges_count += 1;
	}
	list->metal.queued_buffer_accesses_count = 0;

	id<MTLCommandBuffer> command_buffer = (__bridge id<MTLCommandBuffer>)list->metal.command_buffer;
	[command_buffer commit];
	set_next_fence(device, list->metal.command_buffer);

	id<MTLCommandQueue> command_queue = (__bridge id<MTLCommandQueue>)list->metal.command_queue;
	command_buffer                    = [command_queue commandBuffer];
	list->metal.command_buffer        = (__bridge_retained void *)[command_queue commandBuffer];
}

void kore_metal_device_wait_until_idle(kore_gpu_device *device) {
	wait_for_execution(device, device->metal.execution_fence.next_execution_index - 1);
}

void kore_metal_device_create_descriptor_set_buffer(kore_gpu_device *device, uint64_t encoded_length, kore_gpu_buffer *buffer) {
	id<MTLDevice> metal_device = (__bridge id<MTLDevice>)device->metal.device;

	MTLResourceOptions options      = MTLResourceCPUCacheModeWriteCombined | MTLResourceStorageModeShared;
	id<MTLBuffer>      metal_buffer = [metal_device newBufferWithLength:encoded_length options:options];
	buffer->metal.buffer            = (__bridge_retained void *)metal_buffer;
}

static MTLSamplerAddressMode convert_addressing(kore_gpu_address_mode mode) {
	switch (mode) {
	case KORE_GPU_ADDRESS_MODE_CLAMP_TO_EDGE:
		return MTLSamplerAddressModeClampToEdge;
	case KORE_GPU_ADDRESS_MODE_REPEAT:
		return MTLSamplerAddressModeRepeat;
	case KORE_GPU_ADDRESS_MODE_MIRROR_REPEAT:
		return MTLSamplerAddressModeMirrorRepeat;
	default:
		assert(false);
		return MTLSamplerAddressModeRepeat;
	}
}

static MTLSamplerMipFilter convert_mipmap_mode(kore_gpu_mipmap_filter_mode filter) {
	switch (filter) {
	case KORE_GPU_MIPMAP_FILTER_MODE_NEAREST:
		return MTLSamplerMipFilterNearest;
	case KORE_GPU_MIPMAP_FILTER_MODE_LINEAR:
		return MTLSamplerMipFilterLinear;
	default:
		assert(false);
		return MTLSamplerMipFilterNearest;
	}
}

static MTLSamplerMinMagFilter convert_texture_filter(kore_gpu_filter_mode filter) {
	switch (filter) {
	case KORE_GPU_FILTER_MODE_NEAREST:
		return MTLSamplerMinMagFilterNearest;
	case KORE_GPU_FILTER_MODE_LINEAR:
		return MTLSamplerMinMagFilterLinear;
	default:
		assert(false);
		return MTLSamplerMinMagFilterNearest;
	}
}

void kore_metal_device_create_sampler(kore_gpu_device *device, const kore_gpu_sampler_parameters *parameters, kore_gpu_sampler *sampler) {
	MTLSamplerDescriptor *desc  = [[MTLSamplerDescriptor alloc] init];
	desc.minFilter              = convert_texture_filter(parameters->min_filter);
	desc.magFilter              = convert_texture_filter(parameters->mag_filter);
	desc.sAddressMode           = convert_addressing(parameters->address_mode_u);
	desc.tAddressMode           = convert_addressing(parameters->address_mode_v);
	desc.mipFilter              = convert_mipmap_mode(parameters->mipmap_filter);
	desc.maxAnisotropy          = parameters->max_anisotropy == 0 ? 1 : parameters->max_anisotropy;
	desc.normalizedCoordinates  = YES;
	desc.lodMinClamp            = parameters->lod_min_clamp;
	desc.lodMaxClamp            = parameters->lod_max_clamp;
	desc.supportArgumentBuffers = true;

	id<MTLDevice> metal_device = (__bridge id<MTLDevice>)device->metal.device;
	sampler->metal.sampler     = (__bridge_retained void *)[metal_device newSamplerStateWithDescriptor:desc];
}

void kore_metal_device_create_raytracing_volume(kore_gpu_device *device, kore_gpu_buffer *vertex_buffer, uint64_t vertex_count, kore_gpu_buffer *index_buffer,
                                                uint32_t index_count, kore_gpu_raytracing_volume *volume) {}

void kore_metal_device_create_raytracing_hierarchy(kore_gpu_device *device, kore_gpu_raytracing_volume **volumes, kore_matrix4x4 *volume_transforms,
                                                   uint32_t volumes_count, kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_metal_device_create_query_set(kore_gpu_device *device, const kore_gpu_query_set_parameters *parameters, kore_gpu_query_set *query_set) {}

uint32_t kore_metal_device_align_texture_row_bytes(kore_gpu_device *device, uint32_t row_bytes) {
	return row_bytes;
}

void kore_metal_device_create_fence(kore_gpu_device *device, kore_gpu_fence *fence) {}

void kore_metal_device_signal(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}

void kore_metal_device_wait(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}
