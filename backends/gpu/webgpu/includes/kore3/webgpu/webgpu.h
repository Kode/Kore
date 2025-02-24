#ifndef KORE_WEBGPU_HEADER
#define KORE_WEBGPU_HEADER

#ifdef KINC_EMSCRIPTEN

#include <webgpu/webgpu.h>

#else

typedef struct WGPUDevice {
	int nothing;
} WGPUDevice;

typedef struct WGPUQueue {
	int nothing;
} WGPUQueue;

typedef struct WGPUSwapChain {
	int nothing;
} WGPUSwapChain;

typedef enum WGPUSType {
	WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector
} WGPUSType;

typedef struct WGPUChainedStruct {
	WGPUSType sType;
} WGPUChainedStruct;

typedef struct WGPUSurfaceDescriptorFromCanvasHTMLSelector {
	WGPUChainedStruct chain;
	const char *selector;
} WGPUSurfaceDescriptorFromCanvasHTMLSelector;

typedef struct WGPUSurfaceDescriptor {
	WGPUChainedStruct *nextInChain;
} WGPUSurfaceDescriptor;

typedef void *WGPUInstance;

typedef struct WGPUSurface {
	int nothing;
} WGPUSurface;

typedef enum WGPUTextureUsage {
	WGPUTextureUsage_RenderAttachment
} WGPUTextureUsage;

typedef enum WGPUTextureFormat {
	WGPUTextureFormat_BGRA8Unorm
} WGPUTextureFormat;

typedef enum WGPUPresentMode {
	WGPUPresentMode_Fifo
} WGPUPresentMode;

typedef struct WGPUSwapChainDescriptor {
	WGPUTextureUsage usage;
	WGPUTextureFormat format;
    int width;
    int height;
    WGPUPresentMode presentMode;
} WGPUSwapChainDescriptor;

static inline WGPUQueue wgpuDeviceGetQueue(WGPUDevice device) {
	WGPUQueue queue = {
		.nothing = 0,
	};
	return queue;
}

static inline WGPUSurface wgpuInstanceCreateSurface(WGPUInstance instance, const WGPUSurfaceDescriptor *surfaceDescriptor) {
	WGPUSurface surface = {
		.nothing = 0,
	};
	return surface;
}

static inline WGPUSwapChain wgpuDeviceCreateSwapChain(WGPUDevice device, WGPUSurface surface, const WGPUSwapChainDescriptor* swapChainDescriptor) {
	WGPUSwapChain swapChain = {
		.nothing = 0,
	};
	return swapChain;
}

#endif

#endif
