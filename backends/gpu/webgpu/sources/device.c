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

static WGPUDevice   wgpu_device;
static WGPUInstance wgpu_instance;
static WGPUAdapter  wgpu_adapter;

static void error_callback(WGPUErrorType errorType, const char *message, void *userdata) {
	kore_log(KORE_LOG_LEVEL_ERROR, "%d: %s", errorType, message);
}

static void compilation_info_callback(WGPUCompilationInfoRequestStatus status, const WGPUCompilationInfo *info, void *userdata) {
	assert(status == WGPUCompilationInfoRequestStatus_Success);
	assert(info->messageCount == 0);
	kore_log(KORE_LOG_LEVEL_INFO, "Shader compile succeeded");
}

void kore_webgpu_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist) {
	device->webgpu.device = wgpu_device;

	wgpuDeviceSetUncapturedErrorCallback(wgpu_device, error_callback, NULL);

	device->webgpu.queue = wgpuDeviceGetQueue(device->webgpu.device);

	WGPUSurfaceDescriptorFromCanvasHTMLSelector canvas_selector = {
	    .selector = "#canvas",
	    .chain =
	        {
	            .sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector,
	        },
	};

	WGPUSurfaceDescriptor surface_descriptor = {
	    .nextInChain = (WGPUChainedStruct *)&canvas_selector,
	};
	device->webgpu.surface = wgpuInstanceCreateSurface(wgpu_instance, &surface_descriptor);

	WGPUSurfaceCapabilities capabilities = {0};
	wgpuSurfaceGetCapabilities(device->webgpu.surface, wgpu_adapter, &capabilities);

	WGPUSurfaceConfiguration surface_configuration = {
	    .device      = wgpu_device,
	    .format      = capabilities.formats[0],
	    .usage       = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_CopySrc | WGPUTextureUsage_CopyDst,
	    .alphaMode   = WGPUCompositeAlphaMode_Auto,
	    .width       = kore_window_width(0),
	    .height      = kore_window_height(0),
	    .presentMode = WGPUPresentMode_Fifo,
	};
	wgpuSurfaceConfigure(device->webgpu.surface, &surface_configuration);

	WGPUShaderModuleWGSLDescriptor shader_module_wgsl_descriptor = {
	    .code        = wgsl,
	    .chain.sType = WGPUSType_ShaderModuleWGSLDescriptor,
	};

	WGPUShaderModuleDescriptor shader_module_descriptor = {
	    .nextInChain = (WGPUChainedStruct *)&shader_module_wgsl_descriptor,
	};

	device->webgpu.shader_module = wgpuDeviceCreateShaderModule(device->webgpu.device, &shader_module_descriptor);
	wgpuShaderModuleGetCompilationInfo(device->webgpu.shader_module, compilation_info_callback, NULL);
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
	if ((usage & KORE_WEBGPU_BUFFER_USAGE_VERTEX) != 0) {
		wgpu_usage |= WGPUBufferUsage_Vertex;
	}
	if ((usage & KORE_WEBGPU_BUFFER_USAGE_UNIFORM) != 0) {
		wgpu_usage |= WGPUBufferUsage_Uniform;
	}
	if ((usage & KORE_WEBGPU_BUFFER_USAGE_STORAGE) != 0) {
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
	kore_gpu_buffer_usage usage = parameters->usage_flags;

	buffer->webgpu.write                  = false;
	buffer->webgpu.has_copy_buffer        = false;
	buffer->webgpu.outdated_regions_count = 0;
	buffer->webgpu.locked_data            = NULL;

	if ((usage & KORE_GPU_BUFFER_USAGE_CPU_WRITE) != 0) {
		kore_gpu_buffer_usage usage_without_write = usage ^ KORE_GPU_BUFFER_USAGE_CPU_WRITE;
		buffer->webgpu.has_copy_buffer            = usage_without_write != KORE_GPU_BUFFER_USAGE_COPY_SRC && usage_without_write != 0;

		buffer->webgpu.write = true;
	}

	if (buffer->webgpu.has_copy_buffer) {
		WGPUBufferDescriptor buffer_descriptor = {
		    .size             = align_pow2(parameters->size, 4),
		    .usage            = WGPUBufferUsage_MapWrite | WGPUBufferUsage_CopySrc,
		    .mappedAtCreation = true,
		};

		buffer->webgpu.copy_buffer = wgpuDeviceCreateBuffer(device->webgpu.device, &buffer_descriptor);

		usage ^= KORE_GPU_BUFFER_USAGE_CPU_WRITE;
		if ((usage & KORE_GPU_BUFFER_USAGE_COPY_SRC) != 0) {
			usage ^= KORE_GPU_BUFFER_USAGE_COPY_SRC;
		}
		usage |= KORE_GPU_BUFFER_USAGE_COPY_DST;
	}

	WGPUBufferDescriptor buffer_descriptor = {
	    .size             = align_pow2(parameters->size, 4),
	    .usage            = convert_buffer_usage(usage),
	    .mappedAtCreation = ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_CPU_WRITE) != 0) && !buffer->webgpu.has_copy_buffer,
	};

	buffer->webgpu.buffer = wgpuDeviceCreateBuffer(device->webgpu.device, &buffer_descriptor);

	buffer->webgpu.size = align_pow2(parameters->size, 4);
}

void kore_webgpu_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {
	WGPUCommandEncoderDescriptor command_encoder_descriptor = {0};
	list->webgpu.command_encoder                            = wgpuDeviceCreateCommandEncoder(device->webgpu.device, &command_encoder_descriptor);
	list->webgpu.render_pass_encoder = NULL;
	list->webgpu.compute_pass_encoder = NULL;
}

void kore_webgpu_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {
	WGPUTextureUsageFlags usage = 0;

	if ((parameters->usage & KORE_GPU_TEXTURE_USAGE_COPY_SRC) != 0) {
		usage |= WGPUTextureUsage_CopySrc;
	}

	if ((parameters->usage & KORE_GPU_TEXTURE_USAGE_COPY_DST) != 0) {
		usage |= WGPUTextureUsage_CopyDst;
	}

	if ((parameters->usage & KORE_WEBGPU_TEXTURE_USAGE_SAMPLED) != 0) {
		usage |= WGPUTextureUsage_TextureBinding;
	}

	if ((parameters->usage & KORE_WEBGPU_TEXTURE_USAGE_STORAGE) != 0) {
		usage |= WGPUTextureUsage_StorageBinding;
	}

	if ((parameters->usage & KORE_GPU_TEXTURE_USAGE_RENDER_ATTACHMENT) != 0) {
		usage |= WGPUTextureUsage_RenderAttachment;
	}

	WGPUTextureDescriptor texture_descriptor = {
	    .sampleCount = 1,
	    .format      = kore_webgpu_convert_texture_format(parameters->format),
	    .usage       = usage,
	    .size =
	        {
	            .width              = parameters->width,
	            .height             = parameters->height,
	            .depthOrArrayLayers = parameters->depth_or_array_layers,
	        },
	    .dimension     = WGPUTextureDimension_2D,
	    .mipLevelCount = 1,
	};

	texture->webgpu.texture = wgpuDeviceCreateTexture(device->webgpu.device, &texture_descriptor);
	texture->webgpu.format  = parameters->format;
}

static kore_gpu_texture framebuffer;

kore_gpu_texture *kore_webgpu_device_get_framebuffer(kore_gpu_device *device) {
	WGPUSurfaceTexture surface_texture;
	wgpuSurfaceGetCurrentTexture(device->webgpu.surface, &surface_texture);
	framebuffer.webgpu.texture = surface_texture.texture;
	framebuffer.webgpu.format  = kore_webgpu_device_framebuffer_format(device);
	return &framebuffer;
}

kore_gpu_texture_format kore_webgpu_device_framebuffer_format(kore_gpu_device *device) {
	return KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM;
}

void kore_webgpu_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {
	if (scheduled_buffer_uploads_count > 0) {
		WGPUCommandEncoderDescriptor command_encoder_descriptor = {0};
		WGPUCommandEncoder           buffer_upload_encoder      = wgpuDeviceCreateCommandEncoder(device->webgpu.device, &command_encoder_descriptor);

		for (uint32_t buffer_index = 0; buffer_index < scheduled_buffer_uploads_count; ++buffer_index) {
			buffer_upload *upload = &scheduled_buffer_uploads[buffer_index];
			wgpuCommandEncoderCopyBufferToBuffer(buffer_upload_encoder, upload->buffer->copy_buffer, upload->offset, upload->buffer->buffer, upload->offset,
			                                     upload->size);
		}

		WGPUCommandBufferDescriptor command_buffer_descriptor = {0};
		WGPUCommandBuffer           command_buffer            = wgpuCommandEncoderFinish(buffer_upload_encoder, &command_buffer_descriptor);
		wgpuQueueSubmit(device->webgpu.queue, 1, &command_buffer);

		scheduled_buffer_uploads_count = 0;
	}

	WGPUCommandBufferDescriptor command_buffer_descriptor = {0};
	WGPUCommandBuffer           command_buffer            = wgpuCommandEncoderFinish(list->webgpu.command_encoder, &command_buffer_descriptor);
	wgpuQueueSubmit(device->webgpu.queue, 1, &command_buffer);

	WGPUCommandEncoderDescriptor command_encoder_descriptor = {0};
	list->webgpu.command_encoder                            = wgpuDeviceCreateCommandEncoder(device->webgpu.device, &command_encoder_descriptor);
}

static bool work_done = false;

static void work_done_callback(WGPUQueueWorkDoneStatus status, void *userdata) {
	work_done = true;
}

void kore_webgpu_device_wait_until_idle(kore_gpu_device *device) {
	work_done = false;
	wgpuQueueOnSubmittedWorkDone(device->webgpu.queue, work_done_callback, NULL);
	while (!work_done) {
		emscripten_sleep(0);
	}
}

void kore_webgpu_device_create_descriptor_set(kore_gpu_device *device, WGPUBindGroup bind_group, kore_webgpu_descriptor_set *set) {
	set->bind_group = bind_group;
}

static WGPUAddressMode convert_address_mode(kore_gpu_address_mode mode) {
	switch (mode) {
	case KORE_GPU_ADDRESS_MODE_CLAMP_TO_EDGE:
		return WGPUAddressMode_ClampToEdge;
	case KORE_GPU_ADDRESS_MODE_REPEAT:
		return WGPUAddressMode_Repeat;
	case KORE_GPU_ADDRESS_MODE_MIRROR_REPEAT:
		return WGPUAddressMode_MirrorRepeat;
	}

	assert(false);
	return WGPUAddressMode_Repeat;
}

static WGPUFilterMode convert_filter_mode(kore_gpu_filter_mode mode) {
	switch (mode) {
	case KORE_GPU_FILTER_MODE_NEAREST:
		return WGPUFilterMode_Nearest;
	case KORE_GPU_FILTER_MODE_LINEAR:
		return WGPUFilterMode_Linear;
	}

	assert(false);
	return WGPUFilterMode_Nearest;
}

static WGPUMipmapFilterMode convert_mipmap_filter_mode(kore_gpu_mipmap_filter_mode mode) {
	switch (mode) {
	case KORE_GPU_MIPMAP_FILTER_MODE_NEAREST:
		return WGPUMipmapFilterMode_Nearest;
	case KORE_GPU_MIPMAP_FILTER_MODE_LINEAR:
		return WGPUMipmapFilterMode_Linear;
	}

	assert(false);
	return WGPUMipmapFilterMode_Nearest;
}

void kore_webgpu_device_create_sampler(kore_gpu_device *device, const kore_gpu_sampler_parameters *parameters, kore_gpu_sampler *sampler) {
	WGPUSamplerDescriptor sampler_descriptor = {
	    .addressModeU  = convert_address_mode(parameters->address_mode_u),
	    .addressModeV  = convert_address_mode(parameters->address_mode_v),
	    .addressModeW  = convert_address_mode(parameters->address_mode_w),
	    .magFilter     = convert_filter_mode(parameters->mag_filter),
	    .minFilter     = convert_filter_mode(parameters->min_filter),
	    .mipmapFilter  = convert_mipmap_filter_mode(parameters->mipmap_filter),
	    .lodMinClamp   = parameters->lod_min_clamp,
	    .lodMaxClamp   = parameters->lod_max_clamp,
	    .compare       = convert_compare(parameters->compare),
	    .maxAnisotropy = parameters->max_anisotropy,
	};
	sampler->webgpu.sampler = wgpuDeviceCreateSampler(device->webgpu.device, &sampler_descriptor);
}

void kore_webgpu_device_create_raytracing_volume(kore_gpu_device *device, kore_gpu_buffer *vertex_buffer, uint64_t vertex_count, kore_gpu_buffer *index_buffer,
                                                 uint32_t index_count, kore_gpu_raytracing_volume *volume) {}

void kore_webgpu_device_create_raytracing_hierarchy(kore_gpu_device *device, kore_gpu_raytracing_volume **volumes, kore_matrix4x4 *volume_transforms,
                                                    uint32_t volumes_count, kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_webgpu_device_create_query_set(kore_gpu_device *device, const kore_gpu_query_set_parameters *parameters, kore_gpu_query_set *query_set) {}

uint32_t kore_webgpu_device_align_texture_row_bytes(kore_gpu_device *device, uint32_t row_bytes) {
	return (uint32_t)align_pow2((int)row_bytes, 256);
}

void kore_webgpu_device_create_fence(kore_gpu_device *device, kore_gpu_fence *fence) {}

void kore_webgpu_device_signal(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}

void kore_webgpu_device_wait(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}

static void (*kickstart_callback)();

void device_callback(WGPURequestDeviceStatus status, WGPUDevice device, char const *message, void *userdata) {
	if (message != NULL) {
		kore_log(KORE_LOG_LEVEL_INFO, "RequestDevice: %s", message);
	}
	assert(status == WGPURequestDeviceStatus_Success);

	wgpu_device = device;

	kickstart_callback();
}

void adapter_callback(WGPURequestAdapterStatus status, WGPUAdapter adapter, char const *message, void *userdata) {
	if (message != NULL) {
		kore_log(KORE_LOG_LEVEL_INFO, "RequestAdapter: %s", message);
	}
	assert(status == WGPURequestAdapterStatus_Success);

	wgpu_adapter = adapter;

	WGPUAdapterInfo info;
	wgpuAdapterGetInfo(wgpu_adapter, &info);
	kore_log(KORE_LOG_LEVEL_INFO, "adapter vendor: %s", info.vendor);
	kore_log(KORE_LOG_LEVEL_INFO, "adapter architecture: %s", info.architecture);
	kore_log(KORE_LOG_LEVEL_INFO, "adapter device: %s", info.device);
	kore_log(KORE_LOG_LEVEL_INFO, "adapter description: %s", info.description);

	wgpuAdapterRequestDevice(wgpu_adapter, NULL, device_callback, NULL);
}

void kore_webgpu_init(void (*callback)()) {
	kickstart_callback = callback;

	wgpu_instance = wgpuCreateInstance(NULL);
	wgpuInstanceRequestAdapter(wgpu_instance, NULL, adapter_callback, NULL);
}
