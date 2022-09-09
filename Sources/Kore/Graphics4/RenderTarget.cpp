#include "Graphics.h"

#include <kinc/graphics4/rendertarget.h>

using namespace Kore;
using namespace Kore::Graphics4;

static void copyValues(RenderTarget *renderTarget, kinc_g4_render_target_t *kincRenderTarget) {
	renderTarget->width = kincRenderTarget->width;
	renderTarget->height = kincRenderTarget->height;
	renderTarget->texWidth = kincRenderTarget->texWidth;
	renderTarget->texHeight = kincRenderTarget->texHeight;
	renderTarget->isCubeMap = kincRenderTarget->isCubeMap;
	renderTarget->isDepthAttachment = kincRenderTarget->isDepthAttachment;
}

RenderTarget::RenderTarget(int width, int height, RenderTargetFormat format, int depthBufferBits, int stencilBufferBits, int samplesPerPixel) {
	kinc_g4_render_target_init_with_multisampling(&kincRenderTarget, width, height, (kinc_g4_render_target_format_t)format, depthBufferBits, stencilBufferBits,
	                                              samplesPerPixel);
	copyValues(this, &kincRenderTarget);
}

RenderTarget::RenderTarget(int cubeMapSize, RenderTargetFormat format, int depthBufferBits, int stencilBufferBits, int samplesPerPixel) {
	kinc_g4_render_target_init_cube_with_multisampling(&kincRenderTarget, cubeMapSize, (kinc_g4_render_target_format_t)format, depthBufferBits,
	                                                   stencilBufferBits, samplesPerPixel);
	copyValues(this, &kincRenderTarget);
}

RenderTarget::~RenderTarget() {
	kinc_g4_render_target_destroy(&kincRenderTarget);
}

void RenderTarget::useColorAsTexture(TextureUnit unit) {
	kinc_g4_render_target_use_color_as_texture(&kincRenderTarget, unit.kincUnit);
}

void RenderTarget::useDepthAsTexture(TextureUnit unit) {
	kinc_g4_render_target_use_depth_as_texture(&kincRenderTarget, unit.kincUnit);
}

void RenderTarget::setDepthStencilFrom(RenderTarget *source) {
	kinc_g4_render_target_set_depth_stencil_from(&kincRenderTarget, &source->kincRenderTarget);
}

void RenderTarget::getPixels(u8 *data) {
	kinc_g4_render_target_get_pixels(&kincRenderTarget, data);
}

void RenderTarget::generateMipmaps(int levels) {
	kinc_g4_render_target_generate_mipmaps(&kincRenderTarget, levels);
}
