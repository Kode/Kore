#include <kore3/direct3d11/device_functions.h>

#include "d3d11unit.h"

#include <kore3/gpu/device.h>
#include <kore3/util/align.h>

#include <kore3/log.h>
#include <kore3/system.h>
#include <kore3/window.h>

#include <kore3/backend/microsoft.h>
#ifdef KORE_WINDOWS
#include <kore3/backend/windows.h>
#endif

#include "d3d11unit.h"

#include <assert.h>

static IDXGISwapChain  *swap_chain;
static kore_gpu_texture framebuffer;

#ifdef KORE_WINDOWS
static bool isWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor) {
	OSVERSIONINFOEXW osvi = {sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0};
	DWORDLONG const  dwlConditionMask =
	    VerSetConditionMask(VerSetConditionMask(VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL), VER_MINORVERSION, VER_GREATER_EQUAL),
	                        VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

	osvi.dwMajorVersion    = wMajorVersion;
	osvi.dwMinorVersion    = wMinorVersion;
	osvi.wServicePackMajor = wServicePackMajor;

	return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}

#ifndef _WIN32_WINNT_WIN8
#define _WIN32_WINNT_WIN8 0x0602
#endif

#ifndef _WIN32_WINNT_WIN10
#define _WIN32_WINNT_WIN10 0x0A00
#endif

static bool isWindows8OrGreater(void) {
	return isWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8), LOBYTE(_WIN32_WINNT_WIN8), 0);
}

static bool isWindows10OrGreater(void) {
	return isWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN10), LOBYTE(_WIN32_WINNT_WIN10), 0);
}
#endif

void kore_d3d11_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist) {
	D3D_FEATURE_LEVEL feature_levels[] = {
#ifdef KORE_WINDOWSAPP
	    D3D_FEATURE_LEVEL_11_1,
#endif
	    D3D_FEATURE_LEVEL_11_0,
	    D3D_FEATURE_LEVEL_10_1,
	    D3D_FEATURE_LEVEL_10_0,
	};
	UINT flags = 0;

#ifndef NDEBUG
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	IDXGIDevice  *dxgi_device;
	IDXGIAdapter *dxgi_adapter;
	IDXGIFactory *dxgi_factory;

	D3D_FEATURE_LEVEL feature_level;

	IDXGIAdapter *adapter = NULL;
	HRESULT       result = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, feature_levels, sizeof(feature_levels) / sizeof(feature_levels[0]),
	                                         D3D11_SDK_VERSION, &device->d3d11.device, &feature_level, &device->d3d11.context);

#ifndef NDEBUG
	if (result == E_FAIL || result == DXGI_ERROR_SDK_COMPONENT_MISSING) {
		kore_log(KORE_LOG_LEVEL_WARNING, "%s", "Failed to create device with D3D11_CREATE_DEVICE_DEBUG, trying without");
		flags ^= D3D11_CREATE_DEVICE_DEBUG;
		result = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, feature_levels, sizeof(feature_levels) / sizeof(feature_levels[0]),
		                           D3D11_SDK_VERSION, &device->d3d11.device, &feature_level, &device->d3d11.context);
	}
#endif

	if (result != S_OK) {
		kore_log(KORE_LOG_LEVEL_WARNING, "%s", "Falling back to the WARP driver, things will be slow.");
		kore_microsoft_affirm(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_WARP, NULL, flags, feature_levels, sizeof(feature_levels) / sizeof(feature_levels[0]),
		                                        D3D11_SDK_VERSION, &device->d3d11.device, &feature_level, &device->d3d11.context));
	}
	kore_microsoft_affirm(device->d3d11.device->lpVtbl->QueryInterface(device->d3d11.device, &IID_IDXGIDevice, (void **)&dxgi_device));
	kore_microsoft_affirm(dxgi_device->lpVtbl->GetAdapter(dxgi_device, &dxgi_adapter));
	kore_microsoft_affirm(dxgi_adapter->lpVtbl->GetParent(dxgi_adapter, &IID_IDXGIFactory, (void **)&dxgi_factory));

	int  depth_bits   = 24;
	int  stencil_bits = 8;
	bool vsync        = true;

	const int _DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL = 3;
	const int _DXGI_SWAP_EFFECT_FLIP_DISCARD    = 4;

	DXGI_SWAP_EFFECT swap_effect;

#ifdef KORE_WINDOWS
	if (isWindows10OrGreater()) {
		swap_effect = DXGI_SWAP_EFFECT_DISCARD;
		//(DXGI_SWAP_EFFECT) _DXGI_SWAP_EFFECT_FLIP_DISCARD;
	}
	else if (isWindows8OrGreater()) {
		swap_effect = DXGI_SWAP_EFFECT_DISCARD;
		//(DXGI_SWAP_EFFECT) _DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	}
	else
#endif
	{
		swap_effect = DXGI_SWAP_EFFECT_DISCARD;
	}

	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {
	    .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
	    .BufferDesc =
	        {
	            .RefreshRate =
	                {
	                    .Denominator = 1, // 60Hz
	                    .Numerator   = 60,
	                },
	            .Width            = kore_window_width(0), // use automatic sizing
	            .Height           = kore_window_height(0),
	            .Format           = DXGI_FORMAT_B8G8R8A8_UNORM, // this is the most common swapchain format
	            .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
	            .Scaling          = DXGI_MODE_SCALING_UNSPECIFIED, // DXGI_SCALING_NONE
	        },
	    // .Stereo = false,
	    .SampleDesc =
	        {
	            .Count   = 1,
	            .Quality = 0,
	        },
	    .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
	    .BufferCount = 2, // use two buffers to enable flip effect
	    .SwapEffect  = swap_effect,
	    .Flags       = 0,
#ifdef KORE_WINDOWS
	    .OutputWindow = kore_windows_window_handle(0),
#endif
	    .Windowed = true,
	};

	kore_microsoft_affirm(dxgi_factory->lpVtbl->CreateSwapChain(dxgi_factory, (IUnknown *)dxgi_device, &swap_chain_desc, &swap_chain));

	swap_chain->lpVtbl->ResizeBuffers(swap_chain, 1, kore_window_width(0), kore_window_height(0), DXGI_FORMAT_B8G8R8A8_UNORM, 0);

	kore_microsoft_affirm(swap_chain->lpVtbl->GetBuffer(swap_chain, 0, &IID_ID3D11Texture2D, (void **)&framebuffer.d3d11.texture));

	kore_microsoft_affirm(device->d3d11.device->lpVtbl->CreateRenderTargetView(device->d3d11.device, (ID3D11Resource *)framebuffer.d3d11.texture, NULL,
	                                                                           &framebuffer.d3d11.render_target_view));

	D3D11_TEXTURE2D_DESC backbuffer_desc;
	framebuffer.d3d11.texture->lpVtbl->GetDesc(framebuffer.d3d11.texture, &backbuffer_desc);
	framebuffer.d3d11.width  = backbuffer_desc.Width;
	framebuffer.d3d11.height = backbuffer_desc.Height;
}

void kore_d3d11_device_destroy(kore_gpu_device *device) {}

void kore_d3d11_device_set_name(kore_gpu_device *device, const char *name) {}

void kore_d3d11_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer) {
	D3D11_BUFFER_DESC buffer_desc = {
	    .Usage               = D3D11_USAGE_DYNAMIC,
	    .ByteWidth           = (UINT)parameters->size,
	    .BindFlags           = 0,
	    .CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE,
	    .MiscFlags           = 0,
	    .StructureByteStride = 0,
	};

	if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_INDEX) != 0) {
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	}
	else if ((parameters->usage_flags & KORE_D3D11_BUFFER_USAGE_VERTEX) != 0) {
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	}
	else if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_CPU_READ) != 0) {
		buffer_desc.Usage          = D3D11_USAGE_DEFAULT;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		buffer_desc.BindFlags      = D3D11_BIND_UNORDERED_ACCESS;
	}

	kore_microsoft_affirm(device->d3d11.device->lpVtbl->CreateBuffer(device->d3d11.device, &buffer_desc, NULL, &buffer->d3d11.buffer));

	buffer->d3d11.context = device->d3d11.context;
}

void kore_d3d11_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {
	list->d3d11.commands        = malloc(1024 * 1024);
	list->d3d11.commands_offset = 0;
}

void kore_d3d11_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {}

kore_gpu_texture *kore_d3d11_device_get_framebuffer(kore_gpu_device *device) {
	return &framebuffer;
}

kore_gpu_texture_format kore_d3d11_device_framebuffer_format(kore_gpu_device *device) {
	return KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM;
}

void kore_d3d11_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {
	uint64_t offset = 0;

	while (offset < list->d3d11.commands_offset) {
		command *c = (command *)&list->d3d11.commands[offset];

		switch (c->type) {
		case COMMAND_SET_INDEX_BUFFER: {
			set_index_buffer_data *data = (set_index_buffer_data *)&c->data;

			device->d3d11.context->lpVtbl->IASetIndexBuffer(device->d3d11.context, data->buffer->d3d11.buffer,
			                                                data->index_format == KORE_GPU_INDEX_FORMAT_UINT16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT,
			                                                (UINT)data->offset);

			break;
		}
		case COMMAND_SET_VERTEX_BUFFER: {
			set_vertex_buffer_data *data = (set_vertex_buffer_data *)&c->data;

			ID3D11Buffer *buffers[1] = {data->buffer->buffer};
			UINT          strides[1] = {(UINT)data->stride};
			UINT          offsets[1] = {(UINT)data->offset};

			device->d3d11.context->lpVtbl->IASetVertexBuffers(device->d3d11.context, 0, 1, buffers, strides, offsets);

			break;
		}
		case COMMAND_DRAW_INDEXED: {
			draw_indexed_data *data = (draw_indexed_data *)&c->data;

			device->d3d11.context->lpVtbl->IASetPrimitiveTopology(device->d3d11.context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			device->d3d11.context->lpVtbl->DrawIndexed(device->d3d11.context, data->index_count, data->first_index, data->base_vertex);

			break;
		}
		case COMMAND_SET_RENDER_PIPELINE: {
			set_render_pipeline *data = (set_render_pipeline *)&c->data;

			device->d3d11.context->lpVtbl->OMSetDepthStencilState(device->d3d11.context, data->pipeline->depth_stencil_state, 0);

			device->d3d11.context->lpVtbl->RSSetState(device->d3d11.context, data->pipeline->rasterizer_state);

			device->d3d11.context->lpVtbl->VSSetShader(device->d3d11.context, data->pipeline->vertex_shader, NULL, 0);
			device->d3d11.context->lpVtbl->PSSetShader(device->d3d11.context, data->pipeline->fragment_shader, NULL, 0);

			device->d3d11.context->lpVtbl->IASetInputLayout(device->d3d11.context, data->pipeline->input_layout);

			break;
		}
		case COMMAND_COPY_TEXTURE_TO_BUFFER: {
			copy_texture_to_buffer *data = (copy_texture_to_buffer *)&c->data;

			break;
		}
		case COMMAND_BEGIN_RENDER_PASS: {
			begin_render_pass *data = (begin_render_pass *)&c->data;

			device->d3d11.context->lpVtbl->OMSetRenderTargets(device->d3d11.context, 1,
			                                                  &data->parameters.color_attachments[0].texture.texture->d3d11.render_target_view, NULL);

			D3D11_VIEWPORT view_port = {
			    .TopLeftX = 0.0f,
			    .TopLeftY = 0.0f,
			    .Width    = (float)data->parameters.color_attachments[0].texture.texture->d3d11.width,
			    .Height   = (float)data->parameters.color_attachments[0].texture.texture->d3d11.height,
			    .MinDepth = D3D11_MIN_DEPTH,
			    .MaxDepth = D3D11_MAX_DEPTH,
			};
			device->d3d11.context->lpVtbl->RSSetViewports(device->d3d11.context, 1, &view_port);

			break;
		}
		case COMMAND_END_RENDER_PASS: {
			break;
		}
		case COMMAND_PRESENT: {
			HRESULT result = swap_chain->lpVtbl->Present(swap_chain, true, 0);

			while (result == DXGI_STATUS_OCCLUDED) {
				// http://www.pouet.net/topic.php?which=10454
				// "Proper handling of DXGI_STATUS_OCCLUDED would be to pause the application,
				// and periodically call Present with the TEST flag, and when it returns S_OK, resume rendering."

				Sleep(50);
				kore_internal_handle_messages();
				result = swap_chain->lpVtbl->Present(swap_chain, true, DXGI_PRESENT_TEST);
			}

			if (result != S_OK) {
				kore_log(KORE_LOG_LEVEL_WARNING, "Coudld not swap the swap chain");
			}

			if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET) {
				exit(1);
			}
		}
		}

		offset += c->size;
	}

	list->d3d11.commands_offset = 0;
}

void kore_d3d11_device_wait_until_idle(kore_gpu_device *device) {}

void kore_d3d11_device_create_descriptor_set(kore_gpu_device *device, uint32_t descriptor_count, uint32_t dynamic_descriptor_count,
                                             uint32_t bindless_descriptor_count, uint32_t sampler_count, kore_d3d11_descriptor_set *set) {}

void kore_d3d11_device_create_sampler(kore_gpu_device *device, const kore_gpu_sampler_parameters *parameters, kore_gpu_sampler *sampler) {}

void kore_d3d11_device_create_raytracing_volume(kore_gpu_device *device, kore_gpu_buffer *vertex_buffer, uint64_t vertex_count, kore_gpu_buffer *index_buffer,
                                                uint32_t index_count, kore_gpu_raytracing_volume *volume) {}

void kore_d3d11_device_create_raytracing_hierarchy(kore_gpu_device *device, kore_gpu_raytracing_volume **volumes, kore_matrix4x4 *volume_transforms,
                                                   uint32_t volumes_count, kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_d3d11_device_create_query_set(kore_gpu_device *device, const kore_gpu_query_set_parameters *parameters, kore_gpu_query_set *query_set) {}

uint32_t kore_d3d11_device_align_texture_row_bytes(kore_gpu_device *device, uint32_t row_bytes) {
	return row_bytes;
}

void kore_d3d11_device_create_fence(kore_gpu_device *device, kore_gpu_fence *fence) {}

void kore_d3d11_device_signal(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}

void kore_d3d11_device_wait(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}
