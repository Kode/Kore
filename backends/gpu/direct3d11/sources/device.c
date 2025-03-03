#include <kore3/direct3d11/device_functions.h>

#include "d3d11unit.h"

#include <kore3/gpu/device.h>
#include <kore3/util/align.h>

#include <kore3/log.h>
#include <kore3/window.h>

#include <kore3/backend/microsoft.h>
#include <kore3/backend/windows.h>

#include <d3d11.h>
#include <dxgi.h>

#include <assert.h>

#ifdef KORE_WINDOWS
static bool isWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor) {
	OSVERSIONINFOEXW osvi = {sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0};
	DWORDLONG const dwlConditionMask =
	    VerSetConditionMask(VerSetConditionMask(VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL), VER_MINORVERSION, VER_GREATER_EQUAL),
	                        VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

	osvi.dwMajorVersion = wMajorVersion;
	osvi.dwMinorVersion = wMinorVersion;
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
#ifdef KINC_WINDOWSAPP
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

	ID3D11Device *d3d11_device;
	ID3D11DeviceContext *context;
	IDXGIDevice *dxgi_device;
	IDXGIAdapter *dxgi_adapter;
	IDXGIFactory *dxgi_factory;
	D3D_FEATURE_LEVEL feature_level;

	IDXGIAdapter *adapter = NULL;
	HRESULT result = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, feature_levels, sizeof(feature_levels) / sizeof(feature_levels[0]),
	                                   D3D11_SDK_VERSION, &d3d11_device, &feature_level, &context);

#ifndef NDEBUG
	if (result == E_FAIL || result == DXGI_ERROR_SDK_COMPONENT_MISSING) {
		kore_log(KORE_LOG_LEVEL_WARNING, "%s", "Failed to create device with D3D11_CREATE_DEVICE_DEBUG, trying without");
		flags ^= D3D11_CREATE_DEVICE_DEBUG;
		result = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, feature_levels, sizeof(feature_levels) / sizeof(feature_levels[0]),
		                           D3D11_SDK_VERSION, &d3d11_device, &feature_level, &context);
	}
#endif

	if (result != S_OK) {
		kore_log(KORE_LOG_LEVEL_WARNING, "%s", "Falling back to the WARP driver, things will be slow.");
		kore_microsoft_affirm(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_WARP, NULL, flags, feature_levels, sizeof(feature_levels) / sizeof(feature_levels[0]),
		                                        D3D11_SDK_VERSION, &d3d11_device, &feature_level, &context));
	}
	kore_microsoft_affirm(d3d11_device->lpVtbl->QueryInterface(d3d11_device, &IID_IDXGIDevice, (void **)&dxgi_device));
	kore_microsoft_affirm(dxgi_device->lpVtbl->GetAdapter(dxgi_device, &dxgi_adapter));
	kore_microsoft_affirm(dxgi_adapter->lpVtbl->GetParent(dxgi_adapter, &IID_IDXGIFactory, (void **)&dxgi_factory));

	int depth_bits = 24;
	int stencil_bits = 8;
	bool vsync = true;

	const int _DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL = 3;
	const int _DXGI_SWAP_EFFECT_FLIP_DISCARD = 4;

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
	                    .Numerator = 60,
	                },
	            .Width = kore_window_width(0), // use automatic sizing
	            .Height = kore_window_height(0),
	            .Format = DXGI_FORMAT_B8G8R8A8_UNORM, // this is the most common swapchain format
	            .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
	            .Scaling = DXGI_MODE_SCALING_UNSPECIFIED, // DXGI_SCALING_NONE
	        },
	    // .Stereo = false,
	    .SampleDesc =
	        {
	            .Count = 1,
	            .Quality = 0,
	        },
	    .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
	    .BufferCount = 2, // use two buffers to enable flip effect
	    .SwapEffect = swap_effect,
	    .Flags = 0,
	    .OutputWindow = kore_windows_window_handle(0),
	    .Windowed = true,
	};

	IDXGISwapChain *swap_chain;

	kore_microsoft_affirm(dxgi_factory->lpVtbl->CreateSwapChain(dxgi_factory, (IUnknown *)dxgi_device, &swap_chain_desc, &swap_chain));

	swap_chain->lpVtbl->ResizeBuffers(swap_chain, 1, kore_window_width(0), kore_window_height(0), DXGI_FORMAT_B8G8R8A8_UNORM, 0);

	ID3D11Texture2D *backbuffer;
	kore_microsoft_affirm(swap_chain->lpVtbl->GetBuffer(swap_chain, 0, &IID_ID3D11Texture2D, (void **)&backbuffer));

	ID3D11RenderTargetView *render_target_view;
	kore_microsoft_affirm(d3d11_device->lpVtbl->CreateRenderTargetView(d3d11_device, (ID3D11Resource *)&backbuffer, NULL, &render_target_view));

	D3D11_TEXTURE2D_DESC backbuffer_desc;
	backbuffer->lpVtbl->GetDesc(backbuffer, &backbuffer_desc);
	// int width = backbuffer_desc.Width;
	// int height = backbuffer_desc.Height;

	D3D11_VIEWPORT view_port = {
	    .TopLeftX = 0.0f,
	    .TopLeftY = 0.0f,
	    .Width = (float)kore_window_width(0),
	    .Height = (float)kore_window_height(0),
	    .MinDepth = D3D11_MIN_DEPTH,
	    .MaxDepth = D3D11_MAX_DEPTH,
	};
	context->lpVtbl->RSSetViewports(context, 1, &view_port);
}

void kore_d3d11_device_destroy(kore_gpu_device *device) {}

void kore_d3d11_device_set_name(kore_gpu_device *device, const char *name) {}

void kore_d3d11_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer) {}

void kore_d3d11_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {}

void kore_d3d11_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {}

kore_gpu_texture *kore_d3d11_device_get_framebuffer(kore_gpu_device *device) {
	return NULL;
}

kore_gpu_texture_format kore_d3d11_device_framebuffer_format(kore_gpu_device *device) {
	return KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM;
}

void kore_d3d11_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {}

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
	return 0;
}

void kore_d3d11_device_create_fence(kore_gpu_device *device, kore_gpu_fence *fence) {}

void kore_d3d11_device_signal(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}

void kore_d3d11_device_wait(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}
