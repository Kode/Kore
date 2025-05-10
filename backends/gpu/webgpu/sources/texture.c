#include <kore3/webgpu/texture_functions.h>

#include "webgpuunit.h"

#include <kore3/gpu/texture.h>

void kore_webgpu_texture_set_name(kore_gpu_texture *texture, const char *name) {}

void kore_webgpu_texture_destroy(kore_gpu_texture *texture) {}

WGPUTextureFormat kore_webgpu_convert_texture_format(kore_gpu_texture_format format) {
	switch (format) {
	case KORE_GPU_TEXTURE_FORMAT_UNDEFINED:
		assert(false);
		return WGPUTextureFormat_BGRA8Unorm;
	case KORE_GPU_TEXTURE_FORMAT_R8_UNORM:
		return WGPUTextureFormat_R8Unorm;
	case KORE_GPU_TEXTURE_FORMAT_R8_SNORM:
		return WGPUTextureFormat_R8Snorm;
	case KORE_GPU_TEXTURE_FORMAT_R8_UINT:
		return WGPUTextureFormat_R8Uint;
	case KORE_GPU_TEXTURE_FORMAT_R8_SINT:
		return WGPUTextureFormat_R8Sint;
	case KORE_GPU_TEXTURE_FORMAT_R16_UINT:
		return WGPUTextureFormat_R16Uint;
	case KORE_GPU_TEXTURE_FORMAT_R16_SINT:
		return WGPUTextureFormat_R16Sint;
	case KORE_GPU_TEXTURE_FORMAT_R16_FLOAT:
		return WGPUTextureFormat_R16Float;
	case KORE_GPU_TEXTURE_FORMAT_RG8_UNORM:
		return WGPUTextureFormat_RG8Unorm;
	case KORE_GPU_TEXTURE_FORMAT_RG8_SNORM:
		return WGPUTextureFormat_RG8Snorm;
	case KORE_GPU_TEXTURE_FORMAT_RG8_UINT:
		return WGPUTextureFormat_RG8Uint;
	case KORE_GPU_TEXTURE_FORMAT_RG8_SINT:
		return WGPUTextureFormat_RG8Sint;
	case KORE_GPU_TEXTURE_FORMAT_R32_UINT:
		return WGPUTextureFormat_R32Uint;
	case KORE_GPU_TEXTURE_FORMAT_R32_SINT:
		return WGPUTextureFormat_R32Sint;
	case KORE_GPU_TEXTURE_FORMAT_R32_FLOAT:
		return WGPUTextureFormat_R32Float;
	case KORE_GPU_TEXTURE_FORMAT_RG16_UINT:
		return WGPUTextureFormat_RG16Uint;
	case KORE_GPU_TEXTURE_FORMAT_RG16_SINT:
		return WGPUTextureFormat_RG16Sint;
	case KORE_GPU_TEXTURE_FORMAT_RG16_FLOAT:
		return WGPUTextureFormat_RG16Float;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM:
		return WGPUTextureFormat_RGBA8Unorm;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM_SRGB:
		return WGPUTextureFormat_RGBA8UnormSrgb;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SNORM:
		return WGPUTextureFormat_RGBA8Snorm;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UINT:
		return WGPUTextureFormat_RGBA8Uint;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SINT:
		return WGPUTextureFormat_RGBA8Sint;
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM:
		return WGPUTextureFormat_BGRA8Unorm;
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM_SRGB:
		return WGPUTextureFormat_BGRA8UnormSrgb;
	case KORE_GPU_TEXTURE_FORMAT_RGB9E5U_FLOAT:
		return WGPUTextureFormat_RGB9E5Ufloat;
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UINT:
		return WGPUTextureFormat_RGB10A2Uint;
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UNORM:
		return WGPUTextureFormat_RGB10A2Unorm;
	case KORE_GPU_TEXTURE_FORMAT_RG11B10U_FLOAT:
		return WGPUTextureFormat_RG11B10Ufloat;
	case KORE_GPU_TEXTURE_FORMAT_RG32_UINT:
		return WGPUTextureFormat_RG32Uint;
	case KORE_GPU_TEXTURE_FORMAT_RG32_SINT:
		return WGPUTextureFormat_RG32Sint;
	case KORE_GPU_TEXTURE_FORMAT_RG32_FLOAT:
		return WGPUTextureFormat_RG32Float;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_UINT:
		return WGPUTextureFormat_RGBA16Uint;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_SINT:
		return WGPUTextureFormat_RGBA16Sint;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_FLOAT:
		return WGPUTextureFormat_RGBA16Float;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_UINT:
		return WGPUTextureFormat_RGBA32Uint;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_SINT:
		return WGPUTextureFormat_RGBA32Sint;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_FLOAT:
		return WGPUTextureFormat_RGBA32Float;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH16_UNORM:
		return WGPUTextureFormat_Depth16Unorm;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24PLUS_NOTHING8:
		return WGPUTextureFormat_Depth24Plus;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24PLUS_STENCIL8:
		return WGPUTextureFormat_Depth24PlusStencil8;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32FLOAT:
		return WGPUTextureFormat_Depth32Float;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32FLOAT_STENCIL8_NOTHING24:
		return WGPUTextureFormat_Depth32FloatStencil8;
	}

	assert(false);
	return WGPUTextureFormat_BGRA8Unorm;
}
