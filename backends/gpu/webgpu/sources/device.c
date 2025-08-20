#include <kore3/webgpu/device_functions.h>

#include "webgpuunit.h"

#include <kore3/gpu/device.h>
#include <kore3/util/align.h>

#include <kore3/log.h>
#include <kore3/window.h>

#ifdef KORE_EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#include <webgpu/webgpu.h>
#endif

#include <assert.h>

#define FRAMEBUFFER_TEXTURE

static WGPUDevice              wgpu_device;
WGPUInstance                   wgpu_instance;
static WGPUAdapter             wgpu_adapter;
static kore_gpu_texture_format framebuffer_format;
#ifdef FRAMEBUFFER_TEXTURE
static kore_gpu_texture framebuffer_texture;
#endif

static void error_callback(WGPUDevice const *device, WGPUErrorType type, WGPUStringView message, void *userdata1, void *userdata2) {
	kore_log(KORE_LOG_LEVEL_ERROR, "%d: %s", type, message.data);
}

void kore_webgpu_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist) {
	device->webgpu.device = wgpu_device;

	device->webgpu.queue = wgpuDeviceGetQueue(device->webgpu.device);

	WGPUEmscriptenSurfaceSourceCanvasHTMLSelector canvas_selector = {
	    .selector =
	        {
	            .data   = "#canvas",
	            .length = 7,
	        },
	    .chain =
	        {
	            .sType = WGPUSType_EmscriptenSurfaceSourceCanvasHTMLSelector,
	        },
	};

	WGPUSurfaceDescriptor surface_descriptor = {
	    .nextInChain = (WGPUChainedStruct *)&canvas_selector,
	};
	device->webgpu.surface = wgpuInstanceCreateSurface(wgpu_instance, &surface_descriptor);

	WGPUSurfaceCapabilities capabilities = {0};
	wgpuSurfaceGetCapabilities(device->webgpu.surface, wgpu_adapter, &capabilities);

	bool rgba8_found = false;
	for (size_t format_index = 0; format_index < capabilities.formatCount; ++format_index) {
		if (capabilities.formats[format_index] == WGPUTextureFormat_RGBA8Unorm) {
			rgba8_found = true;
			break;
		}
	}

	if (rgba8_found) {
		framebuffer_format = KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM;
	}
	else {
		kore_log(KORE_LOG_LEVEL_WARNING, "Can not use RGBA8Unorm format for the framebuffer, direct writes to the framebuffer might not be possible.");
		framebuffer_format = kore_webgpu_convert_texture_format_to_kore(capabilities.formats[0]);
	}

	WGPUSurfaceConfiguration surface_configuration = {
	    .device      = wgpu_device,
	    .format      = kore_webgpu_convert_texture_format_to_webgpu(framebuffer_format),
	    .usage       = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_CopySrc | WGPUTextureUsage_CopyDst,
	    .alphaMode   = WGPUCompositeAlphaMode_Auto,
	    .width       = kore_window_width(0),
	    .height      = kore_window_height(0),
	    .presentMode = WGPUPresentMode_Fifo,
	};
	wgpuSurfaceConfigure(device->webgpu.surface, &surface_configuration);

#ifdef FRAMEBUFFER_TEXTURE
	kore_gpu_texture_parameters texture_parameters = {
	    .format                = framebuffer_format,
	    .width                 = kore_window_width(0),
	    .height                = kore_window_height(0),
	    .dimension             = KORE_GPU_TEXTURE_DIMENSION_2D,
	    .mip_level_count       = 1,
	    .depth_or_array_layers = 1,
	    .sample_count          = 1,
	    .usage                 = KORE_GPU_TEXTURE_USAGE_RENDER_ATTACHMENT | KORE_GPU_TEXTURE_USAGE_COPY_SRC | KORE_GPU_TEXTURE_USAGE_COPY_DST,
	};
	kore_gpu_device_create_texture(device, &texture_parameters, &framebuffer_texture);
#endif
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
		    .mappedAtCreation = false,
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
	    .mappedAtCreation = false,
	};

	buffer->webgpu.buffer = wgpuDeviceCreateBuffer(device->webgpu.device, &buffer_descriptor);

	buffer->webgpu.size = align_pow2(parameters->size, 4);
}

extern WGPUBindGroupLayout root_constants_set_layout;

void kore_webgpu_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {
	WGPUCommandEncoderDescriptor command_encoder_descriptor = {0};
	list->webgpu.command_encoder                            = wgpuDeviceCreateCommandEncoder(device->webgpu.device, &command_encoder_descriptor);
	list->webgpu.render_pass_encoder                        = NULL;
	list->webgpu.compute_pass_encoder                       = NULL;

	WGPUBufferDescriptor buffer_descriptor = {
	    .size             = KORE_WEBGPU_ROOT_CONSTANTS_SIZE,
	    .usage            = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
	    .mappedAtCreation = false,
	};

	list->webgpu.root_constants_buffer = wgpuDeviceCreateBuffer(device->webgpu.device, &buffer_descriptor);

	const WGPUBindGroupEntry entries[] = {
	    {
	        .binding = 0,
	        .buffer  = list->webgpu.root_constants_buffer,
	        .offset  = 0,
	        .size    = 256,
	    },
	};

	WGPUBindGroupDescriptor bind_group_descriptor = {
	    .layout     = root_constants_set_layout,
	    .entries    = entries,
	    .entryCount = 1,
	};
	list->webgpu.root_constants_bind_group = wgpuDeviceCreateBindGroup(device->webgpu.device, &bind_group_descriptor);

	list->webgpu.root_constants_offset = 0;

	list->webgpu.compute_pipeline = NULL;

	list->webgpu.present = false;
}

void kore_webgpu_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {
	WGPUTextureUsage usage = 0;

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
	    .format      = kore_webgpu_convert_texture_format_to_webgpu(parameters->format),
	    .usage       = usage,
	    .size =
	        {
	            .width              = parameters->width,
	            .height             = parameters->height,
	            .depthOrArrayLayers = parameters->depth_or_array_layers,
	        },
	    .dimension     = WGPUTextureDimension_2D,
	    .mipLevelCount = parameters->mip_level_count,
	};

	texture->webgpu.texture = wgpuDeviceCreateTexture(device->webgpu.device, &texture_descriptor);
	texture->webgpu.format  = parameters->format;
	texture->width          = parameters->width;
	texture->height         = parameters->height;
}

static kore_gpu_texture framebuffer;

kore_gpu_texture *kore_webgpu_device_get_framebuffer(kore_gpu_device *device) {
#ifdef FRAMEBUFFER_TEXTURE
	return &framebuffer_texture;
#else
	WGPUSurfaceTexture surface_texture;
	wgpuSurfaceGetCurrentTexture(device->webgpu.surface, &surface_texture);
	framebuffer.webgpu.texture = surface_texture.texture;
	framebuffer.webgpu.format  = kore_webgpu_device_framebuffer_format(device);

	framebuffer.width  = kore_window_width(0);
	framebuffer.height = kore_window_height(0);

	return &framebuffer;
#endif
}

kore_gpu_texture_format kore_webgpu_device_framebuffer_format(kore_gpu_device *device) {
	return framebuffer_format;
}

void kore_webgpu_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {
	if (list->webgpu.compute_pass_encoder != NULL) {
		wgpuComputePassEncoderEnd(list->webgpu.compute_pass_encoder);
		list->webgpu.compute_pipeline          = NULL;
		list->webgpu.compute_bind_groups_count = 0;
		list->webgpu.compute_pass_encoder      = NULL;
	}

	if (list->webgpu.root_constants_offset > 0) {
		wgpuQueueWriteBuffer(device->webgpu.queue, list->webgpu.root_constants_buffer, 0, &list->webgpu.root_constants_data[0],
		                     list->webgpu.root_constants_offset);

		list->webgpu.root_constants_offset = 0;
	}

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

#ifdef FRAMEBUFFER_TEXTURE
	if (list->webgpu.present) {
		WGPUTexelCopyTextureInfo source_texture = {
		    .texture  = framebuffer_texture.webgpu.texture,
		    .mipLevel = 0,
		    .origin =
		        {
		            .x = 0,
		            .y = 0,
		            .z = 0,
		        },
		    .aspect = WGPUTextureAspect_All,
		};

		WGPUSurfaceTexture surface_texture;
		wgpuSurfaceGetCurrentTexture(device->webgpu.surface, &surface_texture);

		WGPUTexelCopyTextureInfo destination_texture = {
		    .texture  = surface_texture.texture,
		    .mipLevel = 0,
		    .origin =
		        {
		            .x = 0,
		            .y = 0,
		            .z = 0,
		        },
		    .aspect = WGPUTextureAspect_All,
		};

		WGPUExtent3D size = {
		    .width              = framebuffer_texture.width,
		    .height             = framebuffer_texture.height,
		    .depthOrArrayLayers = 1,
		};

		wgpuCommandEncoderCopyTextureToTexture(list->webgpu.command_encoder, &source_texture, &destination_texture, &size);
	}
#endif

	WGPUCommandBufferDescriptor command_buffer_descriptor = {0};
	WGPUCommandBuffer           command_buffer            = wgpuCommandEncoderFinish(list->webgpu.command_encoder, &command_buffer_descriptor);
	wgpuQueueSubmit(device->webgpu.queue, 1, &command_buffer);

	WGPUCommandEncoderDescriptor command_encoder_descriptor = {0};
	list->webgpu.command_encoder                            = wgpuDeviceCreateCommandEncoder(device->webgpu.device, &command_encoder_descriptor);

	list->webgpu.present = false;
}

static bool work_done = false;

static void work_done_callback(WGPUQueueWorkDoneStatus status, WGPUStringView view, void *userdata1, void *userdata2) {
	work_done = true;
}

void kore_webgpu_device_wait_until_idle(kore_gpu_device *device) {
	work_done = false;

	WGPUQueueWorkDoneCallbackInfo callback_info = {
	    .mode     = WGPUCallbackMode_AllowProcessEvents,
	    .callback = work_done_callback,
	};

	wgpuQueueOnSubmittedWorkDone(device->webgpu.queue, callback_info);

	while (!work_done) {
		emscripten_sleep(0);
		wgpuInstanceProcessEvents(wgpu_instance);
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

static void device_callback(WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void *userdata1, void *userdata2) {
	if (message.length > 0) {
		kore_log(KORE_LOG_LEVEL_INFO, "RequestDevice: %s", message.data);
	}
	assert(status == WGPURequestDeviceStatus_Success);

	wgpu_device = device;

	kickstart_callback();
}

static void adapter_callback(WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void *userdata1, void *userdata2) {
	if (message.length > 0) {
		kore_log(KORE_LOG_LEVEL_INFO, "RequestAdapter: %s", message.data);
	}
	assert(status == WGPURequestAdapterStatus_Success);

	wgpu_adapter = adapter;

	WGPUAdapterInfo info = {0};
	wgpuAdapterGetInfo(wgpu_adapter, &info);
	kore_log(KORE_LOG_LEVEL_INFO, "adapter vendor: %s", info.vendor.data);
	kore_log(KORE_LOG_LEVEL_INFO, "adapter architecture: %s", info.architecture.data);
	kore_log(KORE_LOG_LEVEL_INFO, "adapter device: %s", info.device.data);
	kore_log(KORE_LOG_LEVEL_INFO, "adapter description: %s", info.description.data);

	WGPUFeatureName required_features[] = {
	    WGPUFeatureName_Float32Filterable,
	};

	// default values via https://gpuweb.github.io/gpuweb/#limits
	WGPULimits required_limits = {
	    .maxTextureDimension1D                     = 8192,
	    .maxTextureDimension2D                     = 8192,
	    .maxTextureDimension3D                     = 2048,
	    .maxTextureArrayLayers                     = 256,
	    .maxBindGroups                             = 4,
	    .maxBindGroupsPlusVertexBuffers            = 24,
	    .maxBindingsPerBindGroup                   = 1000,
	    .maxDynamicUniformBuffersPerPipelineLayout = 8,
	    .maxDynamicStorageBuffersPerPipelineLayout = 4,
	    .maxSampledTexturesPerShaderStage          = 16,
	    .maxSamplersPerShaderStage                 = 16,
	    .maxStorageBuffersPerShaderStage           = 8,
	    .maxStorageTexturesPerShaderStage          = 4,
	    .maxUniformBuffersPerShaderStage           = 12,
	    .maxUniformBufferBindingSize               = 65536,
	    .maxStorageBufferBindingSize               = 134217728,
	    .minUniformBufferOffsetAlignment           = 256,
	    .minStorageBufferOffsetAlignment           = 256,
	    .maxVertexBuffers                          = 8,
	    .maxBufferSize                             = 268435456,
	    .maxVertexAttributes                       = 16,
	    .maxVertexBufferArrayStride                = 2048,
	    .maxInterStageShaderVariables              = 16,
	    .maxColorAttachments                       = 8,
	    .maxColorAttachmentBytesPerSample          = 32,
	    .maxComputeWorkgroupStorageSize            = 16384,
	    .maxComputeInvocationsPerWorkgroup         = 256,
	    .maxComputeWorkgroupSizeX                  = 256,
	    .maxComputeWorkgroupSizeY                  = 256,
	    .maxComputeWorkgroupSizeZ                  = 64,
	    .maxComputeWorkgroupsPerDimension          = 65535,
	};

	required_limits.maxColorAttachmentBytesPerSample = 48; // enough to write to a cube map

	WGPUUncapturedErrorCallbackInfo error_callback_info = {
	    .callback = error_callback,
	};

	WGPUDeviceDescriptor device_descriptor = {
	    .requiredFeatures            = required_features,
	    .requiredFeatureCount        = sizeof(required_features) / sizeof(required_features[0]),
	    .requiredLimits              = &required_limits,
	    .uncapturedErrorCallbackInfo = error_callback_info,
	};

	WGPURequestDeviceCallbackInfo callback_info = {
	    .mode     = WGPUCallbackMode_AllowProcessEvents,
	    .callback = device_callback,
	};

	wgpuAdapterRequestDevice(wgpu_adapter, &device_descriptor, callback_info);
}

void kore_webgpu_init(void (*callback)()) {
	kickstart_callback = callback;

	wgpu_instance = wgpuCreateInstance(NULL);

	WGPURequestAdapterCallbackInfo callback_info = {
	    .mode     = WGPUCallbackMode_AllowProcessEvents,
	    .callback = adapter_callback,
	};
	wgpuInstanceRequestAdapter(wgpu_instance, NULL, callback_info);
}
