#include "Graphics.h"

#include <kinc/graphics5/graphics.h>

using namespace Kore::Graphics5;

void Kore_Internal_ConvertVertexStructure(kinc_g4_vertex_structure_t *target, const VertexStructure *source);

VertexBuffer::VertexBuffer(int count, const VertexStructure &structure, bool gpuMemory, int instanceDataStepRate) {
	kinc_g4_vertex_structure_t kincStructure;
	kinc_g4_vertex_structure_init(&kincStructure);
	Kore_Internal_ConvertVertexStructure(&kincStructure, &structure);
	kinc_g5_vertex_buffer_init(&kincBuffer, count, &kincStructure, gpuMemory, instanceDataStepRate);
}

VertexBuffer::~VertexBuffer() {
	kinc_g5_vertex_buffer_destroy(&kincBuffer);
}

float *VertexBuffer::lock() {
	return kinc_g5_vertex_buffer_lock_all(&kincBuffer);
}

float *VertexBuffer::lock(int start, int count) {
	return kinc_g5_vertex_buffer_lock(&kincBuffer, start, count);
}

void VertexBuffer::unlock() {
	kinc_g5_vertex_buffer_unlock_all(&kincBuffer);
}

void VertexBuffer::unlock(int count) {
	kinc_g5_vertex_buffer_unlock(&kincBuffer, count);
}

int VertexBuffer::count() {
	return kinc_g5_vertex_buffer_count(&kincBuffer);
}

int VertexBuffer::stride() {
	return kinc_g5_vertex_buffer_stride(&kincBuffer);
}

IndexBuffer::IndexBuffer(int count, bool gpuMemory) {
	kinc_g5_index_buffer_init(&kincBuffer, count, KINC_G5_INDEX_BUFFER_FORMAT_32BIT, gpuMemory);
}

IndexBuffer::~IndexBuffer() {
	kinc_g5_index_buffer_destroy(&kincBuffer);
}

void *IndexBuffer::lock() {
	return kinc_g5_index_buffer_lock_all(&kincBuffer);
}

void *IndexBuffer::lock(int start, int count) {
	return kinc_g5_index_buffer_lock(&kincBuffer, start, count);
}

void IndexBuffer::unlock() {
	kinc_g5_index_buffer_unlock_all(&kincBuffer);
}

void IndexBuffer::unlock(int count) {
	kinc_g5_index_buffer_unlock(&kincBuffer, count);
}

int IndexBuffer::count() {
	return kinc_g5_index_buffer_count(&kincBuffer);
}

RenderTarget::RenderTarget(int width, int height, RenderTargetFormat format, int depthBufferBits, int stencilBufferBits, int samplesPerPixel) {
	kinc_g5_render_target_init_with_multisampling(&kincRenderTarget, width, height, (kinc_g5_render_target_format_t)format, depthBufferBits, stencilBufferBits,
	                                              samplesPerPixel);
}

RenderTarget::RenderTarget(int cubeMapSize, RenderTargetFormat format, int depthBufferBits, int stencilBufferBits, int samplesPerPixel) {
	kinc_g5_render_target_init_cube_with_multisampling(&kincRenderTarget, cubeMapSize, (kinc_g5_render_target_format_t)format, depthBufferBits,
	                                                   stencilBufferBits, samplesPerPixel);
}

RenderTarget::~RenderTarget() {
	kinc_g5_render_target_destroy(&kincRenderTarget);
}

void RenderTarget::setDepthStencilFrom(RenderTarget *source) {
	kinc_g5_render_target_set_depth_stencil_from(&kincRenderTarget, &source->kincRenderTarget);
}

int Kore::Graphics5::antialiasingSamples() {
	return kinc_g5_antialiasing_samples();
}

void Kore::Graphics5::setAntialiasingSamples(int samples) {
	kinc_g5_set_antialiasing_samples(samples);
}

bool Kore::Graphics5::renderTargetsInvertedY() {
	return kinc_g5_render_targets_inverted_y();
}

void Kore::Graphics5::begin(RenderTarget *renderTarget, int window) {
	kinc_g5_begin(&renderTarget->kincRenderTarget, window);
}

void Kore::Graphics5::end(int window) {
	kinc_g5_end(window);
}

bool Kore::Graphics5::swapBuffers() {
	return kinc_g5_swap_buffers();
}

int Kore::Graphics5::maxBoundTextures() {
	return kinc_g5_max_bound_textures();
}

bool Kore::Graphics5::supportsRaytracing() {
	return kinc_g5_supports_raytracing();
}

bool Kore::Graphics5::supportsInstancedRendering() {
	return kinc_g5_supports_instanced_rendering();
}

bool Kore::Graphics5::supportsComputeShaders() {
	return kinc_g5_supports_compute_shaders();
}

bool Kore::Graphics5::supportsBlendConstants() {
	return kinc_g5_supports_blend_constants();
}

bool Kore::Graphics5::supportsNonPow2Textures() {
	return kinc_g5_supports_non_pow2_textures();
}

bool Kore::Graphics5::fullscreen = false;

void Kore::Graphics5::flush() {
	kinc_g5_flush();
}
