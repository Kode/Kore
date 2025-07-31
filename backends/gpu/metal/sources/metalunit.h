#ifndef KORE_METAL_UNIT_HEADER
#define KORE_METAL_UNIT_HEADER

#include <kore3/gpu/device.h>

#include <assert.h>

#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

static id<CAMetalDrawable> drawable = nil;

CAMetalLayer *getMetalLayer(void);

static MTLPixelFormat convert_format(kore_gpu_texture_format format) {
	switch (format) {
	case KORE_GPU_TEXTURE_FORMAT_UNDEFINED:
		return MTLPixelFormatInvalid;
	case KORE_GPU_TEXTURE_FORMAT_R8_UNORM:
		return MTLPixelFormatR8Unorm;
	case KORE_GPU_TEXTURE_FORMAT_R8_SNORM:
		return MTLPixelFormatR8Snorm;
	case KORE_GPU_TEXTURE_FORMAT_R8_UINT:
		return MTLPixelFormatR8Uint;
	case KORE_GPU_TEXTURE_FORMAT_R8_SINT:
		return MTLPixelFormatR8Sint;
	case KORE_GPU_TEXTURE_FORMAT_R16_UINT:
		return MTLPixelFormatR16Uint;
	case KORE_GPU_TEXTURE_FORMAT_R16_SINT:
		return MTLPixelFormatR16Sint;
	case KORE_GPU_TEXTURE_FORMAT_R16_FLOAT:
		return MTLPixelFormatR16Float;
	case KORE_GPU_TEXTURE_FORMAT_RG8_UNORM:
		return MTLPixelFormatRG8Unorm;
	case KORE_GPU_TEXTURE_FORMAT_RG8_SNORM:
		return MTLPixelFormatRG8Snorm;
	case KORE_GPU_TEXTURE_FORMAT_RG8_UINT:
		return MTLPixelFormatRG8Uint;
	case KORE_GPU_TEXTURE_FORMAT_RG8_SINT:
		return MTLPixelFormatRG8Sint;
	case KORE_GPU_TEXTURE_FORMAT_R32_UINT:
		return MTLPixelFormatR32Uint;
	case KORE_GPU_TEXTURE_FORMAT_R32_SINT:
		return MTLPixelFormatR32Sint;
	case KORE_GPU_TEXTURE_FORMAT_R32_FLOAT:
		return MTLPixelFormatR32Float;
	case KORE_GPU_TEXTURE_FORMAT_RG16_UINT:
		return MTLPixelFormatRG16Uint;
	case KORE_GPU_TEXTURE_FORMAT_RG16_SINT:
		return MTLPixelFormatRG16Sint;
	case KORE_GPU_TEXTURE_FORMAT_RG16_FLOAT:
		return MTLPixelFormatRG16Float;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM:
		return MTLPixelFormatRGBA8Unorm;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM_SRGB:
		return MTLPixelFormatRGBA8Unorm_sRGB;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SNORM:
		return MTLPixelFormatRGBA8Unorm;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UINT:
		return MTLPixelFormatRGBA8Uint;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SINT:
		return MTLPixelFormatRGBA8Sint;
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM:
		return MTLPixelFormatBGRA8Unorm;
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM_SRGB:
		return MTLPixelFormatBGRA8Unorm_sRGB;
	case KORE_GPU_TEXTURE_FORMAT_RGB9E5U_FLOAT:
		return MTLPixelFormatRGB9E5Float;
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UINT:
		return MTLPixelFormatRGB10A2Uint;
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UNORM:
		return MTLPixelFormatRGB10A2Unorm;
	case KORE_GPU_TEXTURE_FORMAT_RG11B10U_FLOAT:
		return MTLPixelFormatRG11B10Float;
	case KORE_GPU_TEXTURE_FORMAT_RG32_UINT:
		return MTLPixelFormatRG32Uint;
	case KORE_GPU_TEXTURE_FORMAT_RG32_SINT:
		return MTLPixelFormatRG32Sint;
	case KORE_GPU_TEXTURE_FORMAT_RG32_FLOAT:
		return MTLPixelFormatRG32Float;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_UINT:
		return MTLPixelFormatRGBA16Uint;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_SINT:
		return MTLPixelFormatRGBA16Sint;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_FLOAT:
		return MTLPixelFormatRGBA16Float;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_UINT:
		return MTLPixelFormatRGBA32Uint;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_SINT:
		return MTLPixelFormatRGBA32Sint;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_FLOAT:
		return MTLPixelFormatRGBA32Float;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH16_UNORM:
#ifdef KORE_IOS
		assert(false);
		return MTLPixelFormatRGBA8Unorm;
#else
		return MTLPixelFormatDepth16Unorm; // only in new iOS
#endif
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24_NOTHING8:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24_STENCIL8:
#ifdef KORE_IOS
		assert(false);
		return MTLPixelFormatDepth32Float_Stencil8;
#else
		return MTLPixelFormatDepth24Unorm_Stencil8; // not in iOS
#endif
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32_FLOAT:
		return MTLPixelFormatDepth32Float;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32_FLOAT_STENCIL8_NOTHING24:
		return MTLPixelFormatDepth32Float_Stencil8;
	}
}

static bool has_depth(kore_gpu_texture_format format) {
	switch (format) {
	case KORE_GPU_TEXTURE_FORMAT_DEPTH16_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24_NOTHING8:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24_STENCIL8:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32_FLOAT:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32_FLOAT_STENCIL8_NOTHING24:
		return true;
	default:
		return false;
	}
}

static bool has_stencil(kore_gpu_texture_format format) {
	switch (format) {
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24_NOTHING8:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32_FLOAT_STENCIL8_NOTHING24:
		return true;
	default:
		return false;
	}
}

struct kore_gpu_device;

static uint64_t find_completed_execution(struct kore_gpu_device *device);

static void wait_for_execution(struct kore_gpu_device *device, uint64_t index);

#endif
