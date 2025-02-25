#include <kore3/webgpu/device_functions.h>

#include "webgpuunit.h"

#include <kore3/gpu/device.h>
#include <kore3/util/align.h>

#include <kore3/log.h>
#include <kore3/window.h>

#include <wgsl.h>

#ifdef KORE_EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgpu.h>
#endif

#include <assert.h>

void kore_webgpu_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist) {
#ifdef KORE_EMSCRIPTEN
    device->webgpu.device = emscripten_webgpu_get_device();
#endif
    wgpuDevicePushErrorScope(device->webgpu.device, WGPUErrorFilter_Validation);

    device->webgpu.queue = wgpuDeviceGetQueue(device->webgpu.device);

    WGPUSurfaceDescriptorFromCanvasHTMLSelector canvas_selector = {
        .chain = {
            .sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector,
        },
        .selector = "canvas",
    };

	WGPUSurfaceDescriptor surface_descriptor = {
        .nextInChain = (WGPUChainedStruct *)&canvas_selector,
    };

    WGPUInstance instance = 0;
	WGPUSurface surface = wgpuInstanceCreateSurface(instance, &surface_descriptor);

	WGPUSwapChainDescriptor swap_chain_descriptor = {
        .usage = WGPUTextureUsage_RenderAttachment,
        .format = WGPUTextureFormat_BGRA8Unorm,
        .width = kore_window_width(0),
        .height = kore_window_height(0),
        .presentMode = WGPUPresentMode_Fifo,
    };
	device->webgpu.swap_chain = wgpuDeviceCreateSwapChain(device->webgpu.device, surface, &swap_chain_descriptor);

    WGPUShaderModuleWGSLDescriptor shader_module_wgsl_descriptor = {
	    .code = wgsl,
	    .chain.sType = WGPUSType_ShaderModuleWGSLDescriptor,
    };

	WGPUShaderModuleDescriptor shader_module_descriptor = {
	    .nextInChain = (WGPUChainedStruct *)&shader_module_wgsl_descriptor,
    };

	device->webgpu.shader_module = wgpuDeviceCreateShaderModule(device->webgpu.device, &shader_module_descriptor);
}

void kore_webgpu_device_destroy(kore_gpu_device *device) {}

void kore_webgpu_device_set_name(kore_gpu_device *device, const char *name) {}

static WGPUBufferUsage convert_buffer_usage(kore_gpu_buffer_usage usage) {
    WGPUBufferUsage wgpu_usage = 0;
    
    if ((usage & KORE_GPU_BUFFER_USAGE_CPU_READ) != 0) {
        wgpu_usage |= WGPUBufferUsage_MapRead;
    }
    if ((usage & KORE_GPU_BUFFER_USAGE_CPU_WRITE) != 0) {
        wgpu_usage |= WGPUBufferUsage_MapWrite;
    }
    if ((usage & KORE_GPU_BUFFER_USAGE_COPY_SRC) != 0) {
        wgpu_usage |= WGPUBufferUsage_CopySrc;
    }
	if ((usage & KORE_GPU_BUFFER_USAGE_COPY_DST) != 0) {
        wgpu_usage |= WGPUBufferUsage_CopyDst;
    }
    if ((usage & KORE_GPU_BUFFER_USAGE_INDEX) != 0) {
        wgpu_usage |= WGPUBufferUsage_Index;
    }
    if ((usage & KORE_GPU_BUFFER_USAGE_VERTEX) != 0) {
        wgpu_usage |= WGPUBufferUsage_Vertex;
    }
    if ((usage & KORE_GPU_BUFFER_USAGE_READ_WRITE) != 0) {
        wgpu_usage |= WGPUBufferUsage_Storage;
    }
    if ((usage & KORE_GPU_BUFFER_USAGE_INDIRECT) != 0) {
        wgpu_usage |= WGPUBufferUsage_Indirect;
    }
    if ((usage & KORE_GPU_BUFFER_USAGE_QUERY_RESOLVE) != 0) {
        wgpu_usage |= WGPUBufferUsage_QueryResolve;
    }
    if ((usage & KORE_GPU_BUFFER_USAGE_RAYTRACING_VOLUME) != 0) {
        assert(false);
    }

    return wgpu_usage;
}

void kore_webgpu_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer) {
    WGPUBufferDescriptor buffer_descriptor = {
        .size = align_pow2(parameters->size, 4),
        .usage = convert_buffer_usage(parameters->usage_flags),
        .mappedAtCreation = true,
    };

	buffer->webgpu.buffer = wgpuDeviceCreateBuffer(device->webgpu.device, &buffer_descriptor);
    buffer->webgpu.size = parameters->size;
}

void kore_webgpu_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {
    WGPUCommandEncoderDescriptor command_encoder_descriptor = {0};
	list->webgpu.command_encoder = wgpuDeviceCreateCommandEncoder(device->webgpu.device, &command_encoder_descriptor);
}

void kore_webgpu_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {
    WGPUExtent3D size = {
	    .width = parameters->width,
	    .height = parameters->height,
	    .depthOrArrayLayers = parameters->depth_or_array_layers,
    };

	WGPUTextureDescriptor texture_descriptor = {
        .sampleCount = 1,
        .format = WGPUTextureFormat_BGRA8Unorm,
        .usage = WGPUTextureUsage_RenderAttachment,
        .size = parameters->width * parameters->height * 4,
        .dimension = WGPUTextureDimension_2D,
        .mipLevelCount = 1,
    };
	
    texture->webgpu.texture = wgpuDeviceCreateTexture(device->webgpu.device, &texture_descriptor);
}

static kore_gpu_texture framebuffer;

kore_gpu_texture *kore_webgpu_device_get_framebuffer(kore_gpu_device *device) {
    framebuffer.webgpu.texture = wgpuSwapChainGetCurrentTexture(device->webgpu.swap_chain);
	return &framebuffer;
}

kore_gpu_texture_format kore_webgpu_device_framebuffer_format(kore_gpu_device *device) {
    return KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM;
}

void kore_webgpu_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {
    WGPUCommandBufferDescriptor command_buffer_descriptor = {0};

	WGPUCommandBuffer command_buffer = wgpuCommandEncoderFinish(list->webgpu.command_encoder, &command_buffer_descriptor);
	wgpuQueueSubmit(device->webgpu.queue, 1, &command_buffer);

    WGPUCommandEncoderDescriptor command_encoder_descriptor = {0};
	list->webgpu.command_encoder = wgpuDeviceCreateCommandEncoder(device->webgpu.device, &command_encoder_descriptor);
}

void kore_webgpu_device_wait_until_idle(kore_gpu_device *device) {}

void kore_webgpu_device_create_descriptor_set(kore_gpu_device *device, uint32_t descriptor_count, uint32_t dynamic_descriptor_count,
                                              uint32_t bindless_descriptor_count, uint32_t sampler_count, kore_webgpu_descriptor_set *set) {}

void kore_webgpu_device_create_sampler(kore_gpu_device *device, const kore_gpu_sampler_parameters *parameters, kore_gpu_sampler *sampler) {}

void kore_webgpu_device_create_raytracing_volume(kore_gpu_device *device, kore_gpu_buffer *vertex_buffer, uint64_t vertex_count, kore_gpu_buffer *index_buffer,
                                                 uint32_t index_count, kore_gpu_raytracing_volume *volume) {}

void kore_webgpu_device_create_raytracing_hierarchy(kore_gpu_device *device, kore_gpu_raytracing_volume **volumes, kore_matrix4x4 *volume_transforms,
                                                    uint32_t volumes_count, kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_webgpu_device_create_query_set(kore_gpu_device *device, const kore_gpu_query_set_parameters *parameters, kore_gpu_query_set *query_set) {}

uint32_t kore_webgpu_device_align_texture_row_bytes(kore_gpu_device *device, uint32_t row_bytes) {
	return 0;
}

void kore_webgpu_device_create_fence(kore_gpu_device *device, kore_gpu_fence *fence) {}

void kore_webgpu_device_signal(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}

void kore_webgpu_device_wait(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}
