#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgpu.h>
#include <kinc/graphics5/graphics.h>
#include <kinc/graphics5/pipeline.h>
#include <kinc/log.h>
#include <kinc/math/core.h>
#include <kinc/system.h>
#include <string.h>
#include <webgpu/webgpu.h>

int renderTargetWidth;
int renderTargetHeight;
int newRenderTargetWidth;
int newRenderTargetHeight;

WGPUDevice device;
WGPUQueue queue;
WGPUSwapChain swapChain;

void kinc_g5_internal_destroy_window(int windowId) {}

void kinc_g5_internal_destroy() {}

void kinc_g5_internal_init() {}

void kinc_g5_internal_init_window(int window, int depthBufferBits, int stencilBufferBits, bool vsync) {
	newRenderTargetWidth = renderTargetWidth = kinc_width();
	newRenderTargetHeight = renderTargetHeight = kinc_height();

	device = emscripten_webgpu_get_device();
	queue = wgpuDeviceGetQueue(device);

	WGPUSurfaceDescriptorFromCanvasHTMLSelector canvasDesc;
	memset(&canvasDesc, 0, sizeof(canvasDesc));
	canvasDesc.selector = "canvas";

	WGPUSurfaceDescriptor surfDesc;
	memset(&surfDesc, 0, sizeof(surfDesc));
	surfDesc.nextInChain = &canvasDesc;
	WGPUInstance instance = 0;
	WGPUSurface surface = wgpuInstanceCreateSurface(instance, &surfDesc);

	WGPUSwapChainDescriptor scDesc;
	memset(&scDesc, 0, sizeof(scDesc));
	scDesc.usage = WGPUTextureUsage_RenderAttachment;
	scDesc.format = WGPUTextureFormat_BGRA8Unorm;
	scDesc.width = kinc_width();
	scDesc.height = kinc_height();
	scDesc.presentMode = WGPUPresentMode_Fifo;
	swapChain = wgpuDeviceCreateSwapChain(device, surface, &scDesc);
}

void kinc_g5_draw_indexed_vertices_instanced(int instanceCount) {}

void kinc_g5_draw_indexed_vertices_instanced_from_to(int instanceCount, int start, int count) {}

void kinc_g5_draw_indexed_vertices_instanced_from_to_from(int instanceCount, int start, int count, int vertex_offset) {}

void kinc_g5_begin(kinc_g5_render_target_t *renderTarget, int window) {}

void kinc_g5_end(int window) {}

bool kinc_g5_swap_buffers() {
	return true;
}

void kinc_g5_flush() {}

bool kinc_g5_supports_raytracing() {
	return false;
}

bool kinc_g5_supports_instanced_rendering() {
	return true;
}

bool kinc_g5_supports_compute_shaders() {
	return true;
}

bool kinc_g5_supports_blend_constants() {
	return true;
}

bool kinc_g5_supports_non_pow2_textures() {
	return true;
}

bool kinc_g5_render_targets_inverted_y() {
	return false;
}