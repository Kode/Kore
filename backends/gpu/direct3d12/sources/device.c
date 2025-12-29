#include <kore3/direct3d12/device_functions.h>

#include "d3d12unit.h"

#include <kore3/gpu/device.h>
#include <kore3/util/align.h>

#include <kore3/direct3d12/pipeline_functions.h>
#include <kore3/direct3d12/raytracing_functions.h>

#include <kore3/backend/microsoft.h>
#ifdef KORE_WINDOWS
#include <kore3/backend/windows.h>
#endif

#include <kore3/log.h>
#include <kore3/window.h>

#include <assert.h>

#ifdef KORE_WINDOWS
#include <dxgi1_4.h>
#endif

#if defined(KORE_NVAPI) && !defined(NDEBUG)

#include <nvapi.h>

static void __stdcall nvidia_raytracing_validation_message_callback(void *pUserData, NVAPI_D3D12_RAYTRACING_VALIDATION_MESSAGE_SEVERITY severity,
                                                                    const char *messageCode, const char *message, const char *messageDetails) {
	const char *severityString = "unknown";
	switch (severity) {
	case NVAPI_D3D12_RAYTRACING_VALIDATION_MESSAGE_SEVERITY_ERROR:
		severityString = "error";
		break;
	case NVAPI_D3D12_RAYTRACING_VALIDATION_MESSAGE_SEVERITY_WARNING:
		severityString = "warning";
		break;
	}
	kore_log(KORE_LOG_LEVEL_ERROR, "Ray Tracing Validation message: %s: [%s] %s\n%s", severityString, messageCode, message, messageDetails);
}
#endif

void kore_d3d12_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist) {
#ifndef NDEBUG
	ID3D12Debug *debug = NULL;
	if (D3D12GetDebugInterface(COM_OUT(ID3D12Debug, &debug)) == S_OK) {
		COM_CALL_VOID(debug, EnableDebugLayer);
	}
#endif

#ifdef KORE_WINDOWS
	IDXGIFactory4 *dxgi_factory = NULL;
	kore_microsoft_affirm(CreateDXGIFactory1(&IID_IDXGIFactory4, &dxgi_factory));

	HRESULT result = S_FALSE;

#ifndef KORE_D3D12_FORCE_WARP
	result = D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_12_2, &IID_ID3D12Device, &device->d3d12.device);
	if (result == S_OK) {
		kore_log(KORE_LOG_LEVEL_INFO, "%s", "Direct3D running on feature level 12.2.");
	}

	if (result != S_OK) {
		result = D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_12_1, &IID_ID3D12Device, &device->d3d12.device);
		if (result == S_OK) {
			kore_log(KORE_LOG_LEVEL_INFO, "%s", "Direct3D running on feature level 12.1.");
		}
	}

	if (result != S_OK) {
		result = D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_12_0, &IID_ID3D12Device, &device->d3d12.device);
		if (result == S_OK) {
			kore_log(KORE_LOG_LEVEL_INFO, "%s", "Direct3D running on feature level 12.0.");
		}
	}
#endif

	if (result != S_OK) {
		kore_log(KORE_LOG_LEVEL_WARNING, "%s", "Falling back to the WARP driver, things will be slow.");

		IDXGIAdapter *adapter;
		COM_CALL(dxgi_factory, EnumWarpAdapter, &IID_IDXGIAdapter, &adapter);

		result = D3D12CreateDevice((IUnknown *)adapter, D3D_FEATURE_LEVEL_12_2, &IID_ID3D12Device, &device->d3d12.device);
		if (result == S_OK) {
			kore_log(KORE_LOG_LEVEL_INFO, "%s", "Direct3D running on feature level 12.2.");
		}

		if (result != S_OK) {
			result = D3D12CreateDevice((IUnknown *)adapter, D3D_FEATURE_LEVEL_12_1, &IID_ID3D12Device, &device->d3d12.device);
			if (result == S_OK) {
				kore_log(KORE_LOG_LEVEL_INFO, "%s", "Direct3D running on feature level 12.1.");
			}
		}

		if (result != S_OK) {
			result = D3D12CreateDevice((IUnknown *)adapter, D3D_FEATURE_LEVEL_12_0, &IID_ID3D12Device, &device->d3d12.device);
			if (result == S_OK) {
				kore_log(KORE_LOG_LEVEL_INFO, "%s", "Direct3D running on feature level 12.0.");
			}
		}
	}

	assert(result == S_OK);
#else
	kore_d3d12_turbo_create_device(&device->d3d12.device);
#endif

#if defined(KORE_NVAPI) && !defined(NDEBUG)
	NvAPI_Initialize();
	NvAPI_D3D12_EnableRaytracingValidation(device->d3d12.device, NVAPI_D3D12_RAYTRACING_VALIDATION_FLAG_NONE);
	void *handle = nullptr;
	NvAPI_D3D12_RegisterRaytracingValidationMessageCallback(device->d3d12.device, &nvidia_raytracing_validation_message_callback, nullptr, &handle);
#endif

	{
		D3D12_COMMAND_QUEUE_DESC desc = {0};
		desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;

		kore_microsoft_affirm(COM_CREATE(device->d3d12.device, CreateCommandQueue, ID3D12CommandQueue, &device->d3d12.graphics_queue, &desc));
	}

	{
		D3D12_COMMAND_QUEUE_DESC desc = {0};
		desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Type                     = D3D12_COMMAND_LIST_TYPE_COMPUTE;

		kore_microsoft_affirm(COM_CREATE(device->d3d12.device, CreateCommandQueue, ID3D12CommandQueue, &device->d3d12.compute_queue, &desc));
	}

	{
		D3D12_COMMAND_QUEUE_DESC desc = {0};
		desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Type                     = D3D12_COMMAND_LIST_TYPE_COPY;

		kore_microsoft_affirm(COM_CREATE(device->d3d12.device, CreateCommandQueue, ID3D12CommandQueue, &device->d3d12.copy_queue, &desc));
	}

#ifdef KORE_WINDOWS
	{
		DXGI_SWAP_CHAIN_DESC desc = {0};
		desc.BufferCount          = 2;
		desc.BufferDesc.Format    = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferUsage          = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferDesc.Width     = kore_window_width(0);
		desc.BufferDesc.Height    = kore_window_height(0);
		desc.OutputWindow         = kore_windows_window_handle(0);
		desc.SampleDesc.Count     = 1;
		desc.SwapEffect           = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Windowed             = true;

		kore_microsoft_affirm(COM_CALL(dxgi_factory, CreateSwapChain, (IUnknown *)device->d3d12.graphics_queue, &desc, &device->d3d12.swap_chain));
	}

	COM_CALL(dxgi_factory, Release);
#endif

	device->d3d12.cbv_srv_uav_increment = COM_CALL(device->d3d12.device, GetDescriptorHandleIncrementSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	device->d3d12.sampler_increment     = COM_CALL(device->d3d12.device, GetDescriptorHandleIncrementSize, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {0};
		desc.NumDescriptors             = KORE_INDEX_ALLOCATOR_SIZE;
		desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		kore_microsoft_affirm(COM_CREATE(device->d3d12.device, CreateDescriptorHeap, ID3D12DescriptorHeap, &device->d3d12.all_samplers, &desc));

		kore_index_allocator_init(&device->d3d12.sampler_index_allocator);
	}

	for (int i = 0; i < KORE_D3D12_FRAME_COUNT; ++i) {
#ifdef KORE_WINDOWS
		COM_CALL(device->d3d12.swap_chain, GetBuffer, i, &IID_ID3D12Resource, &device->d3d12.framebuffer_textures[i].d3d12.resource);
#else
		kore_d3d12_turbo_create_framebuffer(device->d3d12.device, &device->d3d12.framebuffer_textures[i].d3d12.resource);
#endif

		device->d3d12.framebuffer_textures[i].width                       = kore_window_width(0);
		device->d3d12.framebuffer_textures[i].height                      = kore_window_height(0);
		device->d3d12.framebuffer_textures[i].d3d12.depth_or_array_layers = 1;
		device->d3d12.framebuffer_textures[i].d3d12.mip_level_count       = 1;

		device->d3d12.framebuffer_textures[i].d3d12.in_flight_frame_index = 0;

		device->d3d12.framebuffer_textures[i].format = KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM;

		kore_gpu_texture_parameters parameters = {0};
		parameters.format                      = KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM;
		parameters.dimension                   = KORE_GPU_TEXTURE_DIMENSION_2D;
		parameters.usage                       = KORE_GPU_TEXTURE_USAGE_RENDER_ATTACHMENT;

		device->d3d12.framebuffer_textures[i].d3d12.resource_states[0] = D3D12_RESOURCE_STATE_PRESENT;
	}

	device->d3d12.framebuffer_index = 0;

	COM_CREATE(device->d3d12.device, CreateFence, ID3D12Fence, &device->d3d12.frame_fence, 0, D3D12_FENCE_FLAG_NONE);
	device->d3d12.frame_event         = CreateEvent(NULL, FALSE, FALSE, NULL);
	device->d3d12.current_frame_index = 1;

	COM_CREATE(device->d3d12.device, CreateFence, ID3D12Fence, &device->d3d12.execution_fence, 0, D3D12_FENCE_FLAG_NONE);
	device->d3d12.execution_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	device->d3d12.execution_index = 1;

	{
		const uint32_t descriptor_count = 1024 * 10;

		D3D12_DESCRIPTOR_HEAP_DESC desc = {0};
		desc.NumDescriptors             = descriptor_count;
		desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		kore_microsoft_affirm(COM_CREATE(device->d3d12.device, CreateDescriptorHeap, ID3D12DescriptorHeap, &device->d3d12.descriptor_heap, &desc));

		oa_create(&device->d3d12.descriptor_heap_allocator, descriptor_count, 4096);
	}

	{
		const uint32_t sampler_count = 1024;

		D3D12_DESCRIPTOR_HEAP_DESC desc = {0};
		desc.NumDescriptors             = sampler_count;
		desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		kore_microsoft_affirm(COM_CREATE(device->d3d12.device, CreateDescriptorHeap, ID3D12DescriptorHeap, &device->d3d12.sampler_heap, &desc));

		oa_create(&device->d3d12.sampler_heap_allocator, sampler_count, 4096);
	}

	device->d3d12.render_pipelines_count  = 0;
	device->d3d12.compute_pipelines_count = 0;
	device->d3d12.ray_pipelines_count     = 0;

	memset(&device->d3d12.garbage_buffers, 0, sizeof(device->d3d12.garbage_buffers));
	memset(&device->d3d12.garbage_textures, 0, sizeof(device->d3d12.garbage_textures));
	memset(&device->d3d12.garbage_raytracing_hierarchies, 0, sizeof(device->d3d12.garbage_raytracing_hierarchies));
	memset(&device->d3d12.garbage_raytracing_volumes, 0, sizeof(device->d3d12.garbage_raytracing_volumes));
	memset(&device->d3d12.garbage_command_lists, 0, sizeof(device->d3d12.garbage_command_lists));
	memset(&device->d3d12.garbage_descriptor_sets, 0, sizeof(device->d3d12.garbage_descriptor_sets));
}

void kore_d3d12_device_destroy_buffer(kore_gpu_device *device, kore_gpu_buffer *buffer) {
	if (!kore_d3d12_buffer_in_use(buffer)) {
		COM_CALL_VOID(buffer->d3d12.resource, Release);
		return;
	}

	for (size_t buffer_index = 0; buffer_index < KORE_D3D12_GARBAGE_SIZE; ++buffer_index) {
		if (device->d3d12.garbage_buffers[buffer_index] == NULL) {
			device->d3d12.garbage_buffers[buffer_index] = buffer;
			return;
		}
	}

	assert(false);
}

void kore_d3d12_device_destroy_texture(kore_gpu_device *device, kore_gpu_texture *texture) {
	if (!kore_d3d12_texture_in_use(texture)) {
		COM_CALL_VOID(texture->d3d12.resource, Release);
		return;
	}

	for (size_t texture_index = 0; texture_index < KORE_D3D12_GARBAGE_SIZE; ++texture_index) {
		if (device->d3d12.garbage_textures[texture_index] == NULL) {
			device->d3d12.garbage_textures[texture_index] = texture;
			return;
		}
	}

	assert(false);
}

void kore_d3d12_device_destroy_raytracing_volume(kore_gpu_device *device, kore_gpu_raytracing_volume *volume) {
	if (!kore_d3d12_raytracing_volume_in_use(volume)) {
		COM_CALL_VOID(volume->d3d12.scratch_buffer.d3d12.resource, Release);
		COM_CALL_VOID(volume->d3d12.acceleration_structure.d3d12.resource, Release);
		return;
	}

	for (size_t volume_index = 0; volume_index < KORE_D3D12_GARBAGE_SIZE; ++volume_index) {
		if (device->d3d12.garbage_raytracing_volumes[volume_index] == NULL) {
			device->d3d12.garbage_raytracing_volumes[volume_index] = volume;
			return;
		}
	}

	assert(false);
}

void kore_d3d12_device_destroy_raytracing_hierarchy(kore_gpu_device *device, kore_gpu_raytracing_hierarchy *hierarchy) {
	if (!kore_d3d12_raytracing_hierarchy_in_use(hierarchy)) {
		COM_CALL_VOID(hierarchy->d3d12.instances.d3d12.resource, Release);
		COM_CALL_VOID(hierarchy->d3d12.scratch_buffer.d3d12.resource, Release);
		COM_CALL_VOID(hierarchy->d3d12.update_scratch_buffer.d3d12.resource, Release);
		COM_CALL_VOID(hierarchy->d3d12.acceleration_structure.d3d12.resource, Release);
		return;
	}

	for (size_t hierarchy_index = 0; hierarchy_index < KORE_D3D12_GARBAGE_SIZE; ++hierarchy_index) {
		if (device->d3d12.garbage_raytracing_hierarchies[hierarchy_index] == NULL) {
			device->d3d12.garbage_raytracing_hierarchies[hierarchy_index] = hierarchy;
			return;
		}
	}

	assert(false);
}

void kore_d3d12_device_add_render_pipeline(kore_d3d12_device *device, kore_d3d12_render_pipeline *pipeline) {
	device->render_pipelines[device->render_pipelines_count] = pipeline;
	device->render_pipelines_count += 1;
}

void kore_d3d12_device_add_compute_pipeline(kore_d3d12_device *device, kore_d3d12_compute_pipeline *pipeline) {
	device->compute_pipelines[device->compute_pipelines_count] = pipeline;
	device->compute_pipelines_count += 1;
}

void kore_d3d12_device_add_ray_pipeline(kore_d3d12_device *device, struct kore_d3d12_ray_pipeline *pipeline) {
	device->ray_pipelines[device->ray_pipelines_count] = pipeline;
	device->ray_pipelines_count += 1;
}

void kore_d3d12_device_destroy_command_list(kore_d3d12_device *device, kore_gpu_command_list *list) {
	bool completed = true;
	for (uint32_t allocator_index = 0; allocator_index < KORE_D3D12_COMMAND_LIST_ALLOCATOR_COUNT; ++allocator_index) {
		completed &= check_for_fence(device->execution_fence, list->d3d12.allocator_execution_index[allocator_index]);
	}

	if (completed) {
		for (int i = 0; i < KORE_D3D12_COMMAND_LIST_ALLOCATOR_COUNT; ++i) {
			COM_CALL_VOID(list->d3d12.allocator[i], Release);
		}
		COM_CALL_VOID(list->d3d12.list, Release);

		COM_CALL_VOID(list->d3d12.rtv_descriptors, Release);
		COM_CALL_VOID(list->d3d12.dsv_descriptor, Release);

		return;
	}

	for (size_t list_index = 0; list_index < KORE_D3D12_GARBAGE_SIZE; ++list_index) {
		if (device->garbage_command_lists[list_index] == NULL) {
			device->garbage_command_lists[list_index] = list;
			return;
		}
	}

	assert(false);
}

void kore_d3d12_device_destroy_descriptor_set(kore_gpu_device *device, kore_d3d12_descriptor_set *set) {
	for (uint32_t allocation_index = 0; allocation_index < KORE_D3D12_ALLOCATIONS_PER_DESCRIPTORSET; ++allocation_index) {
		kore_d3d12_descriptor_set_allocation *allocation = &set->allocations[allocation_index];

		if (allocation->command_list_reference_count == 0 &&
		    allocation->execution_index <= COM_CALL_VOID(set->device->d3d12.execution_fence, GetCompletedValue)) {
			oa_free(&device->d3d12.descriptor_heap_allocator, &allocation->descriptor_allocation);
			oa_free(&device->d3d12.descriptor_heap_allocator, &allocation->bindless_descriptor_allocation);
			oa_free(&device->d3d12.sampler_heap_allocator, &allocation->sampler_allocation);

			allocation->descriptor_allocation.index          = OA_INVALID_INDEX;
			allocation->bindless_descriptor_allocation.index = OA_INVALID_INDEX;
			allocation->sampler_allocation.index             = OA_INVALID_INDEX;
		}
	}

	for (size_t set_index = 0; set_index < KORE_D3D12_GARBAGE_SIZE; ++set_index) {
		if (device->d3d12.garbage_descriptor_sets[set_index] == NULL) {
			device->d3d12.garbage_descriptor_sets[set_index] = set;
			return;
		}
	}

	assert(false);
}

static void collect_garbage(kore_gpu_device *device, bool force) {
	for (size_t buffer_index = 0; buffer_index < KORE_D3D12_GARBAGE_SIZE; ++buffer_index) {
		kore_gpu_buffer *buffer = device->d3d12.garbage_buffers[buffer_index];

		if (buffer != NULL) {
			if (force) {
				kore_d3d12_buffer_wait_until_not_in_use(buffer);
				COM_CALL_VOID(buffer->d3d12.resource, Release);
				device->d3d12.garbage_buffers[buffer_index] = NULL;
			}
			else {
				if (!kore_d3d12_buffer_in_use(buffer)) {
					COM_CALL_VOID(buffer->d3d12.resource, Release);
					device->d3d12.garbage_buffers[buffer_index] = NULL;
				}
			}
		}
	}

	for (size_t texture_index = 0; texture_index < KORE_D3D12_GARBAGE_SIZE; ++texture_index) {
		kore_gpu_texture *texture = device->d3d12.garbage_textures[texture_index];

		if (texture != NULL) {
			if (force) {
				kore_d3d12_texture_wait_until_not_in_use(texture);
				COM_CALL_VOID(texture->d3d12.resource, Release);
				device->d3d12.garbage_textures[texture_index] = NULL;
			}
			else {
				if (!kore_d3d12_texture_in_use(texture)) {
					COM_CALL_VOID(texture->d3d12.resource, Release);
					device->d3d12.garbage_textures[texture_index] = NULL;
				}
			}
		}
	}

	for (size_t volume_index = 0; volume_index < KORE_D3D12_GARBAGE_SIZE; ++volume_index) {
		kore_gpu_raytracing_volume *volume = device->d3d12.garbage_raytracing_volumes[volume_index];

		if (volume != NULL) {
			if (force) {
				kore_d3d12_raytracing_volume_wait_until_not_in_use(volume);

				COM_CALL_VOID(volume->d3d12.scratch_buffer.d3d12.resource, Release);
				COM_CALL_VOID(volume->d3d12.acceleration_structure.d3d12.resource, Release);

				device->d3d12.garbage_raytracing_volumes[volume_index] = NULL;
			}
			else {
				if (!kore_d3d12_raytracing_volume_in_use(volume)) {
					COM_CALL_VOID(volume->d3d12.scratch_buffer.d3d12.resource, Release);
					COM_CALL_VOID(volume->d3d12.acceleration_structure.d3d12.resource, Release);

					device->d3d12.garbage_raytracing_volumes[volume_index] = NULL;
				}
			}
		}
	}

	for (size_t hierarchy_index = 0; hierarchy_index < KORE_D3D12_GARBAGE_SIZE; ++hierarchy_index) {
		kore_gpu_raytracing_hierarchy *hierarchy = device->d3d12.garbage_raytracing_hierarchies[hierarchy_index];

		if (hierarchy != NULL) {
			if (force) {
				kore_d3d12_raytracing_hierarchy_wait_until_not_in_use(hierarchy);

				COM_CALL_VOID(hierarchy->d3d12.instances.d3d12.resource, Release);
				COM_CALL_VOID(hierarchy->d3d12.scratch_buffer.d3d12.resource, Release);
				COM_CALL_VOID(hierarchy->d3d12.update_scratch_buffer.d3d12.resource, Release);
				COM_CALL_VOID(hierarchy->d3d12.acceleration_structure.d3d12.resource, Release);

				device->d3d12.garbage_raytracing_hierarchies[hierarchy_index] = NULL;
			}
			else {
				if (!kore_d3d12_raytracing_hierarchy_in_use(hierarchy)) {
					COM_CALL_VOID(hierarchy->d3d12.instances.d3d12.resource, Release);
					COM_CALL_VOID(hierarchy->d3d12.scratch_buffer.d3d12.resource, Release);
					COM_CALL_VOID(hierarchy->d3d12.update_scratch_buffer.d3d12.resource, Release);
					COM_CALL_VOID(hierarchy->d3d12.acceleration_structure.d3d12.resource, Release);

					device->d3d12.garbage_raytracing_hierarchies[hierarchy_index] = NULL;
				}
			}
		}
	}

	for (size_t list_index = 0; list_index < KORE_D3D12_GARBAGE_SIZE; ++list_index) {
		kore_gpu_command_list *list = device->d3d12.garbage_command_lists[list_index];

		if (list != NULL) {
			bool completed = true;
			for (uint32_t allocator_index = 0; allocator_index < KORE_D3D12_COMMAND_LIST_ALLOCATOR_COUNT; ++allocator_index) {
				if (force) {
					wait_for_fence(device, device->d3d12.execution_fence, device->d3d12.execution_event,
					               list->d3d12.allocator_execution_index[allocator_index]);
				}
				else {
					completed &= check_for_fence(device->d3d12.execution_fence, list->d3d12.allocator_execution_index[allocator_index]);
				}
			}

			if (completed) {
				for (int i = 0; i < KORE_D3D12_COMMAND_LIST_ALLOCATOR_COUNT; ++i) {
					COM_CALL_VOID(list->d3d12.allocator[i], Release);
				}
				COM_CALL_VOID(list->d3d12.list, Release);

				COM_CALL_VOID(list->d3d12.rtv_descriptors, Release);
				COM_CALL_VOID(list->d3d12.dsv_descriptor, Release);

				device->d3d12.garbage_command_lists[list_index] = NULL;
			}
		}
	}

	for (size_t set_index = 0; set_index < KORE_D3D12_GARBAGE_SIZE; ++set_index) {
		kore_d3d12_descriptor_set *set = device->d3d12.garbage_descriptor_sets[set_index];

		if (set != NULL) {
			for (uint32_t allocation_index = 0; allocation_index < KORE_D3D12_ALLOCATIONS_PER_DESCRIPTORSET; ++allocation_index) {
				kore_d3d12_descriptor_set_allocation *allocation = &set->allocations[allocation_index];

				if (force) {
					wait_for_fence(device, device->d3d12.execution_fence, device->d3d12.execution_event, allocation->execution_index);

					oa_free(&device->d3d12.descriptor_heap_allocator, &allocation->descriptor_allocation);
					oa_free(&device->d3d12.descriptor_heap_allocator, &allocation->bindless_descriptor_allocation);
					oa_free(&device->d3d12.sampler_heap_allocator, &allocation->sampler_allocation);

					allocation->descriptor_allocation.index          = OA_INVALID_INDEX;
					allocation->bindless_descriptor_allocation.index = OA_INVALID_INDEX;
					allocation->sampler_allocation.index             = OA_INVALID_INDEX;
				}
				else {
					if (allocation->command_list_reference_count == 0 &&
					    allocation->execution_index <= COM_CALL_VOID(set->device->d3d12.execution_fence, GetCompletedValue)) {
						oa_free(&device->d3d12.descriptor_heap_allocator, &allocation->descriptor_allocation);
						oa_free(&device->d3d12.descriptor_heap_allocator, &allocation->bindless_descriptor_allocation);
						oa_free(&device->d3d12.sampler_heap_allocator, &allocation->sampler_allocation);

						allocation->descriptor_allocation.index          = OA_INVALID_INDEX;
						allocation->bindless_descriptor_allocation.index = OA_INVALID_INDEX;
						allocation->sampler_allocation.index             = OA_INVALID_INDEX;
					}
				}
			}
		}
	}
}

void kore_d3d12_device_destroy(kore_gpu_device *device) {
	collect_garbage(device, true);

	kore_gpu_device_wait_until_idle(device);

	for (size_t pipeline_index = 0; pipeline_index < device->d3d12.render_pipelines_count; ++pipeline_index) {
		kore_d3d12_render_pipeline_destroy(device->d3d12.render_pipelines[pipeline_index]);
	}

	for (size_t pipeline_index = 0; pipeline_index < device->d3d12.compute_pipelines_count; ++pipeline_index) {
		kore_d3d12_compute_pipeline_destroy(device->d3d12.compute_pipelines[pipeline_index]);
	}

	for (size_t pipeline_index = 0; pipeline_index < device->d3d12.ray_pipelines_count; ++pipeline_index) {
		kore_d3d12_ray_pipeline_destroy(device->d3d12.ray_pipelines[pipeline_index]);
	}

	COM_CALL_VOID(device->d3d12.graphics_queue, Release);
	COM_CALL_VOID(device->d3d12.compute_queue, Release);
	COM_CALL_VOID(device->d3d12.copy_queue, Release);
	COM_CALL_VOID(device->d3d12.descriptor_heap, Release);
	COM_CALL_VOID(device->d3d12.sampler_heap, Release);
	COM_CALL_VOID(device->d3d12.frame_fence, Release);
	COM_CALL_VOID(device->d3d12.execution_fence, Release);
	COM_CALL_VOID(device->d3d12.all_samplers, Release);

	for (int i = 0; i < KORE_D3D12_FRAME_COUNT; ++i) {
		COM_CALL_VOID(device->d3d12.framebuffer_textures[i].d3d12.resource, Release);
	}
#ifdef KORE_WINDOWS
	COM_CALL(device->d3d12.swap_chain, Release);
#endif

	CloseHandle(device->d3d12.frame_event);
	CloseHandle(device->d3d12.execution_event);

	COM_CALL_VOID(device->d3d12.device, Release);
}

void kore_d3d12_device_set_name(kore_gpu_device *device, const char *name) {
	wchar_t wstr[1024];
	kore_microsoft_convert_string(wstr, name, 1024);
	COM_CALL(device->d3d12.device, SetName, wstr);
}

void kore_d3d12_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer) {
	D3D12_HEAP_PROPERTIES props;
	props.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	props.CreationNodeMask     = 1;
	props.VisibleNodeMask      = 1;
	if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_CPU_WRITE) != 0) {
		props.Type = D3D12_HEAP_TYPE_UPLOAD;
	}
	else if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_CPU_READ) != 0) {
		props.Type = D3D12_HEAP_TYPE_READBACK;
	}
	else {
		props.Type = D3D12_HEAP_TYPE_DEFAULT;
	}

	D3D12_RESOURCE_DESC desc = {0};
	desc.Dimension           = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment           = 0;
	if ((parameters->usage_flags & KORE_D3D12_BUFFER_USAGE_CBV) != 0) {
		desc.Width = align_pow2((int)parameters->size, 256); // 256 required for CBVs
	}
	else {
		desc.Width = parameters->size;
	}
	desc.Height             = 1;
	desc.DepthOrArraySize   = 1;
	desc.MipLevels          = 1;
	desc.Format             = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags              = D3D12_RESOURCE_FLAG_NONE;

	buffer->d3d12.size = parameters->size;

	if ((parameters->usage_flags & KORE_D3D12_BUFFER_USAGE_UAV) != 0) {
		buffer->d3d12.resource_state = D3D12_RESOURCE_STATE_COMMON;
		desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}
	else if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_CPU_READ) != 0) {
		buffer->d3d12.resource_state = D3D12_RESOURCE_STATE_COPY_DEST;
	}
	else {
		buffer->d3d12.resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;
	}

#ifndef KORE_D3D12_NO_RAYTRACING
	if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_RAYTRACING_VOLUME) != 0) {
		buffer->d3d12.resource_state = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
		desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}
#endif

	buffer->d3d12.device = device;

	buffer->d3d12.ranges_count = 0;

	buffer->d3d12.cpu_read  = (parameters->usage_flags & KORE_GPU_BUFFER_USAGE_CPU_READ) != 0;
	buffer->d3d12.cpu_write = (parameters->usage_flags & KORE_GPU_BUFFER_USAGE_CPU_WRITE) != 0;

	kore_microsoft_affirm(COM_CREATE(device->d3d12.device, CreateCommittedResource, ID3D12Resource, &buffer->d3d12.resource, &props, D3D12_HEAP_FLAG_NONE,
	                                 &desc, (D3D12_RESOURCE_STATES)buffer->d3d12.resource_state, NULL));
}

static uint8_t command_list_oldest_allocator(kore_gpu_command_list *list) {
	uint64_t lowest_execution_index = UINT64_MAX;
	uint8_t  allocator_index        = 255;

	for (uint8_t i = 0; i < KORE_D3D12_COMMAND_LIST_ALLOCATOR_COUNT; ++i) {
		if (list->d3d12.allocator_execution_index[i] < lowest_execution_index) {
			allocator_index        = i;
			lowest_execution_index = list->d3d12.allocator_execution_index[i];
		}
	}

	return allocator_index;
}

static D3D12_COMMAND_LIST_TYPE convert_command_list_type(kore_gpu_command_list_type type) {
	switch (type) {
	case KORE_GPU_COMMAND_LIST_TYPE_GRAPHICS:
		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	case KORE_GPU_COMMAND_LIST_TYPE_COMPUTE:
		return D3D12_COMMAND_LIST_TYPE_COMPUTE;
	case KORE_GPU_COMMAND_LIST_TYPE_COPY:
		return D3D12_COMMAND_LIST_TYPE_COPY;
	}
	return D3D12_COMMAND_LIST_TYPE_DIRECT;
}

void kore_d3d12_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {
	list->d3d12.device = &device->d3d12;

	D3D12_COMMAND_LIST_TYPE list_type = convert_command_list_type(type);

	list->d3d12.list_type = list_type;

	for (int i = 0; i < KORE_D3D12_COMMAND_LIST_ALLOCATOR_COUNT; ++i) {
		kore_microsoft_affirm(COM_CREATE(device->d3d12.device, CreateCommandAllocator, ID3D12CommandAllocator, &list->d3d12.allocator[i], list_type));
		list->d3d12.allocator_execution_index[i] = 0;

		oa_allocate(&device->d3d12.descriptor_heap_allocator, KORE_D3D12_COMMAND_LIST_DYNAMIC_DESCRIPTORS_COUNT,
		            &list->d3d12.dynamic_descriptor_allocations[i]);
		list->d3d12.dynamic_descriptor_offsets[i] = 0;
	}

	list->d3d12.current_allocator_index = 0;

	kore_microsoft_affirm(COM_CREATE(device->d3d12.device, CreateCommandList, ID3D12GraphicsCommandList3, &list->d3d12.list, 0, list_type,
	                                 list->d3d12.allocator[list->d3d12.current_allocator_index], NULL));

	list->d3d12.compute_pipe = NULL;
	list->d3d12.ray_pipe     = NULL;
	list->d3d12.render_pipe  = NULL;

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {0};
		desc.NumDescriptors             = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT;
		desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		kore_microsoft_affirm(COM_CREATE(device->d3d12.device, CreateDescriptorHeap, ID3D12DescriptorHeap, &list->d3d12.rtv_descriptors, &desc));

		list->d3d12.rtv_increment = COM_CALL(device->d3d12.device, GetDescriptorHandleIncrementSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {0};
		desc.NumDescriptors             = 1;
		desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		kore_microsoft_affirm(COM_CREATE(device->d3d12.device, CreateDescriptorHeap, ID3D12DescriptorHeap, &list->d3d12.dsv_descriptor, &desc));

		list->d3d12.dsv_increment = COM_CALL(device->d3d12.device, GetDescriptorHandleIncrementSize, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}

	list->d3d12.occlusion_query_set           = NULL;
	list->d3d12.current_occlusion_query_index = 0;

	list->d3d12.timestamp_query_set                     = NULL;
	list->d3d12.timestamp_beginning_of_pass_write_index = 0;
	list->d3d12.timestamp_end_of_pass_write_index       = 0;

	list->d3d12.blocking_frame_index = 0;

	list->d3d12.queued_buffer_accesses_count               = 0;
	list->d3d12.queued_texture_accesses_count              = 0;
	list->d3d12.queued_raytracing_volume_accesses_count    = 0;
	list->d3d12.queued_raytracing_hierarchy_accesses_count = 0;
	list->d3d12.queued_descriptor_set_accesses_count       = 0;

	list->d3d12.presenting = false;

	ID3D12DescriptorHeap *heaps[] = {list->d3d12.device->descriptor_heap, list->d3d12.device->sampler_heap};
	COM_CALL(list->d3d12.list, SetDescriptorHeaps, 2, heaps);
}

static DXGI_FORMAT convert_texture_format(kore_gpu_texture_format format) {
	switch (format) {
	case KORE_GPU_TEXTURE_FORMAT_UNDEFINED:
		return DXGI_FORMAT_UNKNOWN;
	case KORE_GPU_TEXTURE_FORMAT_R8_UNORM:
		return DXGI_FORMAT_R8_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_R8_SNORM:
		return DXGI_FORMAT_R8_SNORM;
	case KORE_GPU_TEXTURE_FORMAT_R8_UINT:
		return DXGI_FORMAT_R8_UINT;
	case KORE_GPU_TEXTURE_FORMAT_R8_SINT:
		return DXGI_FORMAT_R8_SINT;
	case KORE_GPU_TEXTURE_FORMAT_R16_UINT:
		return DXGI_FORMAT_R16_UINT;
	case KORE_GPU_TEXTURE_FORMAT_R16_SINT:
		return DXGI_FORMAT_R16_SINT;
	case KORE_GPU_TEXTURE_FORMAT_R16_FLOAT:
		return DXGI_FORMAT_R16_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RG8_UNORM:
		return DXGI_FORMAT_R8G8_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_RG8_SNORM:
		return DXGI_FORMAT_R8G8_SNORM;
	case KORE_GPU_TEXTURE_FORMAT_RG8_UINT:
		return DXGI_FORMAT_R8G8_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RG8_SINT:
		return DXGI_FORMAT_R8G8_SINT;
	case KORE_GPU_TEXTURE_FORMAT_R32_UINT:
		return DXGI_FORMAT_R32_UINT;
	case KORE_GPU_TEXTURE_FORMAT_R32_SINT:
		return DXGI_FORMAT_R32_SINT;
	case KORE_GPU_TEXTURE_FORMAT_R32_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RG16_UINT:
		return DXGI_FORMAT_R16G16_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RG16_SINT:
		return DXGI_FORMAT_R16G16_SINT;
	case KORE_GPU_TEXTURE_FORMAT_RG16_FLOAT:
		return DXGI_FORMAT_R16G16_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM_SRGB:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SNORM:
		return DXGI_FORMAT_R8G8B8A8_SNORM;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UINT:
		return DXGI_FORMAT_R8G8B8A8_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SINT:
		return DXGI_FORMAT_R8G8B8A8_SINT;
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM:
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM_SRGB:
		return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	case KORE_GPU_TEXTURE_FORMAT_RGB9E5U_FLOAT:
		return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UINT:
		return DXGI_FORMAT_R10G10B10A2_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UNORM:
		return DXGI_FORMAT_R10G10B10A2_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_RG11B10U_FLOAT:
		return DXGI_FORMAT_R11G11B10_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RG32_UINT:
		return DXGI_FORMAT_R32G32_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RG32_SINT:
		return DXGI_FORMAT_R32G32_SINT;
	case KORE_GPU_TEXTURE_FORMAT_RG32_FLOAT:
		return DXGI_FORMAT_R32G32_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_UINT:
		return DXGI_FORMAT_R16G16B16A16_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_SINT:
		return DXGI_FORMAT_R16G16B16A16_SINT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_FLOAT:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_UINT:
		return DXGI_FORMAT_R32G32B32A32_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_SINT:
		return DXGI_FORMAT_R32G32B32A32_SINT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_FLOAT:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	// case KORE_GPU_TEXTURE_FORMAT_STENCIL8:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH16_UNORM:
		return DXGI_FORMAT_D16_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24_NOTHING8:
		return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24_STENCIL8:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32_FLOAT:
		return DXGI_FORMAT_D32_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32_FLOAT_STENCIL8_NOTHING24:
		return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	}

	assert(false);
	return DXGI_FORMAT_UNKNOWN;
}

static D3D12_RESOURCE_DIMENSION convert_texture_dimension(kore_gpu_texture_dimension dimension) {
	switch (dimension) {
	case KORE_GPU_TEXTURE_DIMENSION_1D:
		return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
	case KORE_GPU_TEXTURE_DIMENSION_2D:
		return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	case KORE_GPU_TEXTURE_DIMENSION_3D:
		return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	}

	assert(false);
	return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
}

void kore_d3d12_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {
	texture->d3d12.device = device;

	DXGI_FORMAT format = convert_texture_format(parameters->format);

	D3D12_HEAP_PROPERTIES heap_properties;
	heap_properties.Type                 = D3D12_HEAP_TYPE_DEFAULT;
	heap_properties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap_properties.CreationNodeMask     = 1;
	heap_properties.VisibleNodeMask      = 1;

	D3D12_RESOURCE_DESC desc;
	desc.Dimension          = convert_texture_dimension(parameters->dimension);
	desc.Alignment          = 0;
	desc.Width              = parameters->width;
	desc.Height             = parameters->height;
	desc.DepthOrArraySize   = parameters->depth_or_array_layers;
	desc.MipLevels          = parameters->mip_level_count;
	desc.Format             = format;
	desc.SampleDesc.Count   = parameters->sample_count;
	desc.SampleDesc.Quality = 0;
	desc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags              = D3D12_RESOURCE_FLAG_NONE;

	D3D12_CLEAR_VALUE  optimizedClearValue;
	D3D12_CLEAR_VALUE *optimizedClearValuePointer = NULL;

	if ((parameters->usage & KORE_GPU_TEXTURE_USAGE_RENDER_ATTACHMENT) != 0) {
		if (kore_gpu_texture_format_is_depth(parameters->format)) {
			desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			optimizedClearValue.DepthStencil.Depth   = 1.0f;
			optimizedClearValue.DepthStencil.Stencil = 0;
		}
		else {
			desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			optimizedClearValue.Color[0] = 0.0f;
			optimizedClearValue.Color[1] = 0.0f;
			optimizedClearValue.Color[2] = 0.0f;
			optimizedClearValue.Color[3] = 1.0f;
		}

		optimizedClearValue.Format = format;

		optimizedClearValuePointer = &optimizedClearValue;
	}

	if ((parameters->usage & KORE_D3D12_TEXTURE_USAGE_UAV) != 0) {
		desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	kore_microsoft_affirm(COM_CREATE(device->d3d12.device, CreateCommittedResource, ID3D12Resource, &texture->d3d12.resource, &heap_properties,
	                                 D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, optimizedClearValuePointer));

	texture->d3d12.format = format;

	texture->width                       = parameters->width;
	texture->height                      = parameters->height;
	texture->d3d12.depth_or_array_layers = parameters->depth_or_array_layers;
	texture->d3d12.mip_level_count       = parameters->mip_level_count;

	texture->d3d12.in_flight_frame_index = 0;
	texture->d3d12.execution_index       = 0;

	for (uint32_t array_layer = 0; array_layer < parameters->depth_or_array_layers; ++array_layer) {
		for (uint32_t mip_level = 0; mip_level < parameters->mip_level_count; ++mip_level) {
			texture->d3d12.resource_states[kore_d3d12_texture_resource_state_index(texture, mip_level, array_layer)] =
			    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		}
	}
}

kore_gpu_texture *kore_d3d12_device_get_framebuffer(kore_gpu_device *device) {
	kore_gpu_texture *framebuffer = &device->d3d12.framebuffer_textures[device->d3d12.framebuffer_index];

	uint32_t window_width = kore_window_width(0);
	if (window_width < 8) {
		window_width = 8;
	}

	uint32_t window_height = kore_window_height(0);
	if (window_height < 8) {
		window_height = 8;
	}

#ifdef KORE_WINDOWS
	if (framebuffer->width != window_width || framebuffer->height != window_height) {
		kore_gpu_device_wait_until_idle(device);

		for (uint32_t index = 0; index < KORE_D3D12_FRAME_COUNT; ++index) {
			COM_CALL(device->d3d12.framebuffer_textures[index].d3d12.resource, Release);
		}

		HRESULT result = COM_CALL(device->d3d12.swap_chain, ResizeBuffers, 0, window_width, window_height,
		                          convert_texture_format(kore_d3d12_device_framebuffer_format(device)), 0);
		assert(SUCCEEDED(result));

		for (uint32_t index = 0; index < KORE_D3D12_FRAME_COUNT; ++index) {
			COM_CALL(device->d3d12.swap_chain, GetBuffer, index, &IID_ID3D12Resource, &device->d3d12.framebuffer_textures[index].d3d12.resource);
			device->d3d12.framebuffer_textures[index].width  = window_width;
			device->d3d12.framebuffer_textures[index].height = window_height;
		}

		device->d3d12.framebuffer_index = 0;
		framebuffer                     = &device->d3d12.framebuffer_textures[device->d3d12.framebuffer_index];
	}
#endif

	return framebuffer;
}

static void wait_for_fence(kore_gpu_device *device, ID3D12Fence *fence, HANDLE event, UINT64 completion_value) {
	if (COM_CALL_VOID(fence, GetCompletedValue) < completion_value) {
		kore_microsoft_affirm(COM_CALL(fence, SetEventOnCompletion, completion_value, event));
		WaitForSingleObject(event, INFINITE);

#if defined(KORE_NVAPI) && !defined(NDEBUG)
		NvAPI_D3D12_FlushRaytracingValidationMessages(device->d3d12.device);
#endif
	}
}

kore_gpu_texture_format kore_d3d12_device_framebuffer_format(kore_gpu_device *device) {
	return KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM;
}

static bool check_for_fence(ID3D12Fence *fence, UINT64 completion_value) {
	// kore_log(KORE_LOG_LEVEL_INFO, "Done: %llu Check: %llu", fence->GetCompletedValue(), completion_value);
	return COM_CALL_VOID(fence, GetCompletedValue) >= completion_value;
}

static void wait_for_frame(kore_gpu_device *device, uint64_t frame_index) {
	wait_for_fence(device, device->d3d12.frame_fence, device->d3d12.frame_event, frame_index);
}

static void clean_buffer_accesses(kore_d3d12_buffer *buffer, uint64_t finished_execution_index) {
	kore_d3d12_buffer_range ranges[KORE_D3D12_MAX_BUFFER_RANGES];
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

void kore_d3d12_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {
	if (list->d3d12.list_type == D3D12_COMMAND_LIST_TYPE_DIRECT && list->d3d12.presenting) {
		kore_gpu_texture *framebuffer = kore_d3d12_device_get_framebuffer(device);
		if (framebuffer->d3d12.resource_states[0] != D3D12_RESOURCE_STATE_PRESENT) {
			D3D12_RESOURCE_BARRIER barrier;
			barrier.Transition.pResource   = framebuffer->d3d12.resource;
			barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.StateBefore = (D3D12_RESOURCE_STATES)framebuffer->d3d12.resource_states[0];
			barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.Subresource = 0;

			COM_CALL(list->d3d12.list, ResourceBarrier, 1, &barrier);

			framebuffer->d3d12.resource_states[0] = D3D12_RESOURCE_STATE_PRESENT;
		}
	}

	if (list->d3d12.list_type == D3D12_COMMAND_LIST_TYPE_DIRECT && list->d3d12.blocking_frame_index > 0) {
		wait_for_frame(device, list->d3d12.blocking_frame_index);
		list->d3d12.blocking_frame_index = 0;
	}

	COM_CALL_VOID(list->d3d12.list, Close);

	for (uint32_t buffer_access_index = 0; buffer_access_index < list->d3d12.queued_buffer_accesses_count; ++buffer_access_index) {
		kore_d3d12_buffer_access access = list->d3d12.queued_buffer_accesses[buffer_access_index];
		kore_d3d12_buffer       *buffer = access.buffer;

		clean_buffer_accesses(buffer, COM_CALL_VOID(device->d3d12.execution_fence, GetCompletedValue));

		assert(buffer->ranges_count < KORE_D3D12_MAX_BUFFER_RANGES);
		buffer->ranges[buffer->ranges_count].execution_index = device->d3d12.execution_index;
		buffer->ranges[buffer->ranges_count].offset          = access.offset;
		buffer->ranges[buffer->ranges_count].size            = access.size;
		buffer->ranges_count += 1;
	}
	list->d3d12.queued_buffer_accesses_count = 0;

	for (uint32_t texture_access_index = 0; texture_access_index < list->d3d12.queued_texture_accesses_count; ++texture_access_index) {
		kore_d3d12_texture *access = list->d3d12.queued_texture_accesses[texture_access_index];
		access->execution_index    = device->d3d12.execution_index > access->execution_index ? device->d3d12.execution_index : access->execution_index;
	}
	list->d3d12.queued_texture_accesses_count = 0;

	for (uint32_t volume_access_index = 0; volume_access_index < list->d3d12.queued_raytracing_volume_accesses_count; ++volume_access_index) {
		kore_d3d12_raytracing_volume *access = list->d3d12.queued_raytracing_volume_accesses[volume_access_index];
		access->execution_index = device->d3d12.execution_index > access->execution_index ? device->d3d12.execution_index : access->execution_index;
	}
	list->d3d12.queued_raytracing_volume_accesses_count = 0;

	for (uint32_t hierarchy_access_index = 0; hierarchy_access_index < list->d3d12.queued_raytracing_hierarchy_accesses_count; ++hierarchy_access_index) {
		kore_d3d12_raytracing_hierarchy *access = list->d3d12.queued_raytracing_hierarchy_accesses[hierarchy_access_index];
		access->execution_index = device->d3d12.execution_index > access->execution_index ? device->d3d12.execution_index : access->execution_index;
	}
	list->d3d12.queued_raytracing_hierarchy_accesses_count = 0;

	for (uint32_t set_access_index = 0; set_access_index < list->d3d12.queued_descriptor_set_accesses_count; ++set_access_index) {
		kore_d3d12_descriptor_set *set = list->d3d12.queued_descriptor_set_accesses[set_access_index];

		set->allocations[set->current_allocation_index].execution_index =
		    device->d3d12.execution_index > set->allocations[set->current_allocation_index].execution_index
		        ? device->d3d12.execution_index
		        : set->allocations[set->current_allocation_index].execution_index;
		set->allocations[set->current_allocation_index].command_list_reference_count -= 1;
	}
	list->d3d12.queued_descriptor_set_accesses_count = 0;

	list->d3d12.allocator_execution_index[list->d3d12.current_allocator_index] = device->d3d12.execution_index;

	ID3D12CommandList *lists[] = {(ID3D12CommandList *)list->d3d12.list};
	if (list->d3d12.list_type == D3D12_COMMAND_LIST_TYPE_DIRECT) {
		COM_CALL(device->d3d12.graphics_queue, ExecuteCommandLists, 1, lists);
		COM_CALL(device->d3d12.graphics_queue, Signal, device->d3d12.execution_fence, device->d3d12.execution_index);
	}
	else if (list->d3d12.list_type == D3D12_COMMAND_LIST_TYPE_COMPUTE) {
		COM_CALL(device->d3d12.compute_queue, ExecuteCommandLists, 1, lists);
		COM_CALL(device->d3d12.compute_queue, Signal, device->d3d12.execution_fence, device->d3d12.execution_index);
	}
	else if (list->d3d12.list_type == D3D12_COMMAND_LIST_TYPE_COPY) {
		COM_CALL(device->d3d12.copy_queue, ExecuteCommandLists, 1, lists);
		COM_CALL(device->d3d12.copy_queue, Signal, device->d3d12.execution_fence, device->d3d12.execution_index);
	}

	uint8_t allocator_index             = command_list_oldest_allocator(list);
	list->d3d12.current_allocator_index = allocator_index;
	uint64_t allocator_execution_index  = list->d3d12.allocator_execution_index[allocator_index];

	wait_for_fence(device, device->d3d12.execution_fence, device->d3d12.execution_event, allocator_execution_index);

	list->d3d12.dynamic_descriptor_offsets[allocator_index] = 0;
	COM_CALL_VOID(list->d3d12.allocator[allocator_index], Reset);
	COM_CALL(list->d3d12.list, Reset, list->d3d12.allocator[allocator_index], NULL);

	ID3D12DescriptorHeap *heaps[] = {list->d3d12.device->descriptor_heap, list->d3d12.device->sampler_heap};
	COM_CALL(list->d3d12.list, SetDescriptorHeaps, 2, heaps);

	if (list->d3d12.list_type == D3D12_COMMAND_LIST_TYPE_DIRECT && list->d3d12.presenting) {
		kore_gpu_texture *framebuffer            = kore_d3d12_device_get_framebuffer(device);
		framebuffer->d3d12.in_flight_frame_index = device->d3d12.current_frame_index;

#ifdef KORE_WINDOWS
		kore_microsoft_affirm(COM_CALL(device->d3d12.swap_chain, Present, 1, 0));
#else
		kore_d3d12_turbo_present(device->d3d12.device, device->d3d12.graphics_queue, framebuffer->d3d12.resource);
#endif

		COM_CALL(device->d3d12.graphics_queue, Signal, device->d3d12.frame_fence, device->d3d12.current_frame_index);

		device->d3d12.current_frame_index += 1;
		device->d3d12.framebuffer_index = (device->d3d12.framebuffer_index + 1) % KORE_D3D12_FRAME_COUNT;

		list->d3d12.presenting = false;
	}

	collect_garbage(device, false);

	device->d3d12.execution_index += 1;
}

void kore_d3d12_device_wait_until_idle(kore_gpu_device *device) {
	wait_for_fence(device, device->d3d12.execution_fence, device->d3d12.execution_event, device->d3d12.execution_index - 1);
}

void kore_d3d12_device_create_descriptor_set(kore_gpu_device *device, uint32_t descriptor_count, uint32_t dynamic_descriptor_count,
                                             uint32_t bindless_descriptor_count, uint32_t sampler_count, kore_d3d12_descriptor_set *set) {
	for (uint32_t allocation_index = 0; allocation_index < KORE_D3D12_ALLOCATIONS_PER_DESCRIPTORSET; ++allocation_index) {
		kore_d3d12_descriptor_set_allocation *allocation = &set->allocations[allocation_index];

		allocation->descriptor_allocation.index          = OA_INVALID_INDEX;
		allocation->bindless_descriptor_allocation.index = OA_INVALID_INDEX;
		allocation->sampler_allocation.index             = OA_INVALID_INDEX;

		if (descriptor_count > 0) {
			int result = oa_allocate(&device->d3d12.descriptor_heap_allocator, descriptor_count, &allocation->descriptor_allocation);
			assert(result == 0);
		}
		allocation->descriptor_count = descriptor_count;

		allocation->dynamic_descriptor_count = dynamic_descriptor_count;

		if (bindless_descriptor_count > 0) {
			int result = oa_allocate(&device->d3d12.descriptor_heap_allocator, bindless_descriptor_count, &allocation->bindless_descriptor_allocation);
			assert(result == 0);
		}
		allocation->bindless_descriptor_count = bindless_descriptor_count;

		if (sampler_count > 0) {
			int result = oa_allocate(&device->d3d12.sampler_heap_allocator, sampler_count, &allocation->sampler_allocation);
			assert(result == 0);
		}
		allocation->sampler_count = sampler_count;

		allocation->execution_index              = 0;
		allocation->command_list_reference_count = 0;
	}

	set->device = device;
}

static D3D12_TEXTURE_ADDRESS_MODE convert_address_mode(kore_gpu_address_mode mode) {
	switch (mode) {
	case KORE_GPU_ADDRESS_MODE_CLAMP_TO_EDGE:
		return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	case KORE_GPU_ADDRESS_MODE_REPEAT:
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	case KORE_GPU_ADDRESS_MODE_MIRROR_REPEAT:
		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	default:
		assert(false);
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	}
}

void kore_d3d12_device_create_sampler(kore_gpu_device *device, const kore_gpu_sampler_parameters *parameters, kore_gpu_sampler *sampler) {
	sampler->d3d12.sampler_index = kore_index_allocator_allocate(&device->d3d12.sampler_index_allocator);

	D3D12_SAMPLER_DESC desc = {0};
	desc.Filter =
	    parameters->max_anisotropy > 1 ? D3D12_FILTER_ANISOTROPIC : convert_filter(parameters->min_filter, parameters->mag_filter, parameters->mipmap_filter);
	desc.AddressU      = convert_address_mode(parameters->address_mode_u);
	desc.AddressV      = convert_address_mode(parameters->address_mode_v);
	desc.AddressW      = convert_address_mode(parameters->address_mode_w);
	desc.MinLOD        = parameters->lod_min_clamp;
	desc.MaxLOD        = parameters->lod_max_clamp;
	desc.MipLODBias    = 0.0f;
	desc.MaxAnisotropy = parameters->max_anisotropy;
	desc.ComparisonFunc =
	    parameters->compare == KORE_GPU_COMPARE_FUNCTION_UNDEFINED ? D3D12_COMPARISON_FUNC_ALWAYS : convert_compare_function(parameters->compare);

	D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle;
	COM_CALL_VOID_RET(device->d3d12.all_samplers, GetCPUDescriptorHandleForHeapStart, descriptor_handle);
	descriptor_handle.ptr += sampler->d3d12.sampler_index * device->d3d12.sampler_increment;

	COM_CALL(device->d3d12.device, CreateSampler, &desc, descriptor_handle);
}

void kore_d3d12_device_create_raytracing_volume(kore_gpu_device *device, kore_gpu_buffer *vertex_buffer, uint64_t vertex_count, kore_gpu_buffer *index_buffer,
                                                uint32_t index_count, kore_gpu_raytracing_volume *volume) {
#ifndef KORE_D3D12_NO_RAYTRACING
	volume->d3d12.device          = device;
	volume->d3d12.execution_index = 0;

	volume->d3d12.vertex_buffer = vertex_buffer;
	volume->d3d12.vertex_count  = vertex_count;
	volume->d3d12.index_buffer  = index_buffer;
	volume->d3d12.index_count   = index_count;

	D3D12_RAYTRACING_GEOMETRY_DESC geometry_desc = {0};

	geometry_desc.Type  = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometry_desc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

	geometry_desc.Triangles.Transform3x4 = 0;

	geometry_desc.Triangles.IndexFormat  = volume->d3d12.index_buffer != NULL ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_UNKNOWN;
	geometry_desc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	geometry_desc.Triangles.IndexCount   = volume->d3d12.index_count;
	geometry_desc.Triangles.VertexCount  = (UINT)volume->d3d12.vertex_count;
	geometry_desc.Triangles.IndexBuffer =
	    volume->d3d12.index_buffer != NULL ? COM_CALL_VOID(volume->d3d12.index_buffer->d3d12.resource, GetGPUVirtualAddress) : 0;
	geometry_desc.Triangles.VertexBuffer.StartAddress  = COM_CALL_VOID(volume->d3d12.vertex_buffer->d3d12.resource, GetGPUVirtualAddress);
	geometry_desc.Triangles.VertexBuffer.StrideInBytes = sizeof(float) * 3;

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {0};
	inputs.Type                                                 = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.Flags                                                = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	inputs.NumDescs                                             = 1;
	inputs.DescsLayout                                          = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.pGeometryDescs                                       = &geometry_desc;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuild_info = {0};
	COM_CALL(device->d3d12.device, GetRaytracingAccelerationStructurePrebuildInfo, &inputs, &prebuild_info);

	kore_gpu_buffer_parameters scratch_params;
	scratch_params.size        = prebuild_info.ScratchDataSizeInBytes;
	scratch_params.usage_flags = KORE_D3D12_BUFFER_USAGE_UAV;
	kore_gpu_device_create_buffer(device, &scratch_params, &volume->d3d12.scratch_buffer); // TODO: delete once not needed

	kore_gpu_buffer_parameters as_params;
	as_params.size        = prebuild_info.ResultDataMaxSizeInBytes;
	as_params.usage_flags = (uint32_t)KORE_D3D12_BUFFER_USAGE_UAV | (uint32_t)KORE_GPU_BUFFER_USAGE_RAYTRACING_VOLUME;
	kore_gpu_device_create_buffer(device, &as_params, &volume->d3d12.acceleration_structure);
#endif
}

void kore_d3d12_device_create_raytracing_hierarchy(kore_gpu_device *device, kore_gpu_raytracing_volume **volumes, kore_matrix4x4 *volume_transforms,
                                                   uint32_t volumes_count, kore_gpu_raytracing_hierarchy *hierarchy) {
#ifndef KORE_D3D12_NO_RAYTRACING
	hierarchy->d3d12.device          = device;
	hierarchy->d3d12.execution_index = 0;

	hierarchy->d3d12.volumes_count = volumes_count;

	kore_gpu_buffer_parameters instances_params;
	instances_params.size        = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * hierarchy->d3d12.volumes_count;
	instances_params.usage_flags = KORE_GPU_BUFFER_USAGE_CPU_WRITE;
	kore_gpu_device_create_buffer(device, &instances_params, &hierarchy->d3d12.instances);

	D3D12_RAYTRACING_INSTANCE_DESC *descs = (D3D12_RAYTRACING_INSTANCE_DESC *)kore_gpu_buffer_lock_all(&hierarchy->d3d12.instances);
	for (uint32_t volume_index = 0; volume_index < hierarchy->d3d12.volumes_count; ++volume_index) {
		hierarchy->d3d12.volumes[volume_index] = &volumes[volume_index]->d3d12;

		memset(&descs[volume_index], 0, sizeof(D3D12_RAYTRACING_INSTANCE_DESC));
		descs[volume_index].InstanceID            = volume_index;
		descs[volume_index].InstanceMask          = 1;
		descs[volume_index].AccelerationStructure = COM_CALL_VOID(volumes[volume_index]->d3d12.acceleration_structure.d3d12.resource, GetGPUVirtualAddress);
	}

	for (uint32_t volume_index = 0; volume_index < hierarchy->d3d12.volumes_count; ++volume_index) {
		descs[volume_index].Transform[0][0] = volume_transforms[volume_index].m[0];
		descs[volume_index].Transform[1][0] = volume_transforms[volume_index].m[1];
		descs[volume_index].Transform[2][0] = volume_transforms[volume_index].m[2];

		descs[volume_index].Transform[0][1] = volume_transforms[volume_index].m[4];
		descs[volume_index].Transform[1][1] = volume_transforms[volume_index].m[5];
		descs[volume_index].Transform[2][1] = volume_transforms[volume_index].m[6];

		descs[volume_index].Transform[0][2] = volume_transforms[volume_index].m[8];
		descs[volume_index].Transform[1][2] = volume_transforms[volume_index].m[9];
		descs[volume_index].Transform[2][2] = volume_transforms[volume_index].m[10];

		descs[volume_index].Transform[0][3] = volume_transforms[volume_index].m[12];
		descs[volume_index].Transform[1][3] = volume_transforms[volume_index].m[13];
		descs[volume_index].Transform[2][3] = volume_transforms[volume_index].m[14];
	}

	kore_gpu_buffer_unlock(&hierarchy->d3d12.instances);

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {0};
	inputs.Type                                                 = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.Flags                                                = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	inputs.NumDescs                                             = hierarchy->d3d12.volumes_count;
	inputs.DescsLayout                                          = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.InstanceDescs                                        = COM_CALL_VOID(hierarchy->d3d12.instances.d3d12.resource, GetGPUVirtualAddress);

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuild_info = {0};
	COM_CALL(device->d3d12.device, GetRaytracingAccelerationStructurePrebuildInfo, &inputs, &prebuild_info);

	kore_gpu_buffer_parameters scratch_params;
	scratch_params.size        = prebuild_info.ScratchDataSizeInBytes;
	scratch_params.usage_flags = KORE_D3D12_BUFFER_USAGE_UAV;
	kore_gpu_device_create_buffer(device, &scratch_params, &hierarchy->d3d12.scratch_buffer); // TODO: delete once not needed

	kore_gpu_buffer_parameters update_scratch_params;
	update_scratch_params.size =
	    prebuild_info.UpdateScratchDataSizeInBytes > 0 ? prebuild_info.UpdateScratchDataSizeInBytes : prebuild_info.ScratchDataSizeInBytes;
	update_scratch_params.usage_flags = KORE_D3D12_BUFFER_USAGE_UAV;
	kore_gpu_device_create_buffer(device, &update_scratch_params, &hierarchy->d3d12.update_scratch_buffer);

	kore_gpu_buffer_parameters as_params;
	as_params.size        = prebuild_info.ResultDataMaxSizeInBytes;
	as_params.usage_flags = (uint32_t)KORE_D3D12_BUFFER_USAGE_UAV | (uint32_t)KORE_GPU_BUFFER_USAGE_RAYTRACING_VOLUME;
	kore_gpu_device_create_buffer(device, &as_params, &hierarchy->d3d12.acceleration_structure);
#endif
}

void kore_d3d12_device_create_query_set(kore_gpu_device *device, const kore_gpu_query_set_parameters *parameters, kore_gpu_query_set *query_set) {
	D3D12_QUERY_HEAP_DESC desc = {0};
	desc.Type                  = parameters->type == KORE_GPU_QUERY_TYPE_OCCLUSION ? D3D12_QUERY_HEAP_TYPE_OCCLUSION : D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
	desc.Count                 = parameters->count;
	desc.NodeMask              = 0;

	query_set->d3d12.query_type = (uint8_t)parameters->type;
	COM_CREATE(device->d3d12.device, CreateQueryHeap, ID3D12QueryHeap, &query_set->d3d12.query_heap, &desc);
}

uint32_t kore_d3d12_device_align_texture_row_bytes(kore_gpu_device *device, uint32_t row_bytes) {
	return (uint32_t)align_pow2((int)row_bytes, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
}

void kore_d3d12_device_create_fence(kore_gpu_device *device, kore_gpu_fence *fence) {
	COM_CREATE(device->d3d12.device, CreateFence, ID3D12Fence, &fence->d3d12.fence, 0, D3D12_FENCE_FLAG_NONE);
}

void kore_d3d12_device_signal(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {
	switch (list_type) {
	case KORE_GPU_COMMAND_LIST_TYPE_GRAPHICS:
		COM_CALL(device->d3d12.graphics_queue, Signal, fence->d3d12.fence, value);
		break;
	case KORE_GPU_COMMAND_LIST_TYPE_COMPUTE:
		COM_CALL(device->d3d12.compute_queue, Signal, fence->d3d12.fence, value);
		break;
	case KORE_GPU_COMMAND_LIST_TYPE_COPY:
		COM_CALL(device->d3d12.copy_queue, Signal, fence->d3d12.fence, value);
		break;
	}
}

void kore_d3d12_device_wait(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {
	switch (list_type) {
	case KORE_GPU_COMMAND_LIST_TYPE_GRAPHICS:
		COM_CALL(device->d3d12.graphics_queue, Wait, fence->d3d12.fence, value);
		break;
	case KORE_GPU_COMMAND_LIST_TYPE_COMPUTE:
		COM_CALL(device->d3d12.compute_queue, Wait, fence->d3d12.fence, value);
		break;
	case KORE_GPU_COMMAND_LIST_TYPE_COPY:
		COM_CALL(device->d3d12.copy_queue, Wait, fence->d3d12.fence, value);
		break;
	}
}
