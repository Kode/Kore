#include <kore3/webgpu/device_functions.h>

#include "webgpuunit.h"

#include <kore3/gpu/device.h>
#include <kore3/util/align.h>

#include <kore3/log.h>
#include <kore3/window.h>

#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgpu.h>

#include <assert.h>

void kore_webgpu_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist) {
    device->webgpu.device = emscripten_webgpu_get_device();
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
}

void kore_webgpu_device_destroy(kore_gpu_device *device) {}

void kore_webgpu_device_set_name(kore_gpu_device *device, const char *name) {}

void kore_webgpu_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer) {}

void kore_webgpu_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {}

void kore_webgpu_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {}

kore_gpu_texture *kore_webgpu_device_get_framebuffer(kore_gpu_device *device) {
	return NULL;
}

kore_gpu_texture_format kore_webgpu_device_framebuffer_format(kore_gpu_device *device) {
    return KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM;
}

void kore_webgpu_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {}

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

void kore_gpu_internal_webgpu_create_shader_module(const void *source, size_t length) {}
