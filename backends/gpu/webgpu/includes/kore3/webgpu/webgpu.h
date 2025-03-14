#ifndef KORE_WEBGPU_HEADER
#define KORE_WEBGPU_HEADER

#ifdef KORE_EMSCRIPTEN

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

typedef enum WGPUSType { WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector } WGPUSType;

typedef struct WGPUChainedStruct {
	WGPUSType sType;
} WGPUChainedStruct;

typedef struct WGPUSurfaceDescriptorFromCanvasHTMLSelector {
	WGPUChainedStruct chain;
	const char       *selector;
} WGPUSurfaceDescriptorFromCanvasHTMLSelector;

typedef struct WGPUSurfaceDescriptor {
	WGPUChainedStruct *nextInChain;
} WGPUSurfaceDescriptor;

typedef void *WGPUInstance;

typedef struct WGPUSurface {
	int nothing;
} WGPUSurface;

typedef enum WGPUTextureUsage { WGPUTextureUsage_RenderAttachment } WGPUTextureUsage;

typedef enum WGPUTextureFormat { WGPUTextureFormat_BGRA8Unorm } WGPUTextureFormat;

typedef enum WGPUPresentMode { WGPUPresentMode_Fifo } WGPUPresentMode;

typedef struct WGPUSwapChainDescriptor {
	WGPUTextureUsage  usage;
	WGPUTextureFormat format;
	int               width;
	int               height;
	WGPUPresentMode   presentMode;
} WGPUSwapChainDescriptor;

typedef struct WGPUCommandEncoderDescriptor {
	int nothing;
} WGPUCommandEncoderDescriptor;

typedef struct WGPUCommandEncoder {
	int nothing;
} WGPUCommandEncoder;

typedef struct WGPUCommandBuffer {
	int nothing;
} WGPUCommandBuffer;

typedef struct WGPUCommandBufferDescriptor {
	int nothing;
} WGPUCommandBufferDescriptor;

typedef struct WGPURenderPassEncoder {
	int nothing;
} WGPURenderPassEncoder;

typedef enum WGPULoadOp { WGPULoadOp_Clear } WGPULoadOp;

typedef enum WGPUStoreOp { WGPUStoreOp_Store } WGPUStoreOp;

typedef struct WGPUColor {
	double r;
	double g;
	double b;
	double a;
} WGPUColor;

typedef struct WGPURenderPassColorAttachment {
	WGPULoadOp  loadOp;
	WGPUStoreOp storeOp;
	WGPUColor   clearValue;
} WGPURenderPassColorAttachment;

typedef struct WGPURenderPassDescriptor {
	uint32_t                       colorAttachmentCount;
	WGPURenderPassColorAttachment *colorAttachments;
} WGPURenderPassDescriptor;

static inline WGPUQueue wgpuDeviceGetQueue(WGPUDevice device) {
	WGPUQueue queue = {0};
	return queue;
}

static inline WGPUSurface wgpuInstanceCreateSurface(WGPUInstance instance, const WGPUSurfaceDescriptor *surfaceDescriptor) {
	WGPUSurface surface = {0};
	return surface;
}

static inline WGPUSwapChain wgpuDeviceCreateSwapChain(WGPUDevice device, WGPUSurface surface, const WGPUSwapChainDescriptor *swapChainDescriptor) {
	WGPUSwapChain swapChain = {0};
	return swapChain;
}

static inline WGPUCommandEncoder wgpuDeviceCreateCommandEncoder(WGPUDevice device, const WGPUCommandEncoderDescriptor *commandEncoderDescriptor) {
	WGPUCommandEncoder encoder = {0};
	return encoder;
}

static inline WGPUCommandBuffer wgpuCommandEncoderFinish(WGPUCommandEncoder commandEncoder, const WGPUCommandBufferDescriptor *commandBufferDescriptor) {
	WGPUCommandBuffer commandBuffer = {0};
	return commandBuffer;
}

static inline void wgpuQueueSubmit(WGPUQueue queue, uint32_t count, WGPUCommandBuffer *commandBuffer) {}

static inline void wgpuRenderPassEncoderDrawIndexed(WGPURenderPassEncoder renderPassEncoder, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
                                                    int32_t baseVertex, uint32_t firstInstance) {}

static inline void wgpuRenderPassEncoderEnd(WGPURenderPassEncoder renderPassEncoder) {}

static inline WGPURenderPassEncoder wgpuCommandEncoderBeginRenderPass(WGPUCommandEncoder commandEncoder, const WGPURenderPassDescriptor *renderPassDescriptor) {
	WGPURenderPassEncoder renderPassEncoder = {0};
	return renderPassEncoder;
}

#endif

void kore_webgpu_init(void (*callback)(void));

#endif
