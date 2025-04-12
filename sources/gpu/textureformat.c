#include <kore3/gpu/textureformat.h>

#include <assert.h>

uint32_t kore_gpu_texture_format_byte_size(kore_gpu_texture_format format) {
	switch (format) {
	case KORE_GPU_TEXTURE_FORMAT_R8_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_R8_SNORM:
	case KORE_GPU_TEXTURE_FORMAT_R8_UINT:
	case KORE_GPU_TEXTURE_FORMAT_R8_SINT:
		return 1u;
	case KORE_GPU_TEXTURE_FORMAT_R16_UINT:
	case KORE_GPU_TEXTURE_FORMAT_R16_SINT:
	case KORE_GPU_TEXTURE_FORMAT_R16_FLOAT:
		return 2u;
	case KORE_GPU_TEXTURE_FORMAT_RG8_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_RG8_SNORM:
	case KORE_GPU_TEXTURE_FORMAT_RG8_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RG8_SINT:
		return 2u;
	case KORE_GPU_TEXTURE_FORMAT_R32_UINT:
	case KORE_GPU_TEXTURE_FORMAT_R32_SINT:
	case KORE_GPU_TEXTURE_FORMAT_R32_FLOAT:
	case KORE_GPU_TEXTURE_FORMAT_RG16_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RG16_SINT:
	case KORE_GPU_TEXTURE_FORMAT_RG16_FLOAT:
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM_SRGB:
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SNORM:
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SINT:
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM_SRGB:
	case KORE_GPU_TEXTURE_FORMAT_RGB9E5U_FLOAT:
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_RG11B10U_FLOAT:
		return 4u;
	case KORE_GPU_TEXTURE_FORMAT_RG32_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RG32_SINT:
	case KORE_GPU_TEXTURE_FORMAT_RG32_FLOAT:
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_SINT:
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_FLOAT:
		return 8u;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_SINT:
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_FLOAT:
		return 16u;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH16_UNORM:
		return 2u;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24PLUS_NOTHING8:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24PLUS_STENCIL8:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32FLOAT:
		return 4u;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32FLOAT_STENCIL8_NOTHING24:
		return 8u;
	}

	assert(false);
	return 0u;
}

kore_gpu_texture_format_type kore_gpu_texture_format_get_type(kore_gpu_texture_format format) {
	switch (format) {
	case KORE_GPU_TEXTURE_FORMAT_R8_UNORM:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_R8_SNORM:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_SNORM;
	case KORE_GPU_TEXTURE_FORMAT_R8_UINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UINT;
	case KORE_GPU_TEXTURE_FORMAT_R8_SINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_INT;
	case KORE_GPU_TEXTURE_FORMAT_R16_UINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UINT;
	case KORE_GPU_TEXTURE_FORMAT_R16_SINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_INT;
	case KORE_GPU_TEXTURE_FORMAT_R16_FLOAT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RG8_UNORM:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_RG8_SNORM:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_SNORM;
	case KORE_GPU_TEXTURE_FORMAT_RG8_UINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RG8_SINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_INT;
	case KORE_GPU_TEXTURE_FORMAT_R32_UINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UINT;
	case KORE_GPU_TEXTURE_FORMAT_R32_SINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_INT;
	case KORE_GPU_TEXTURE_FORMAT_R32_FLOAT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RG16_UINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RG16_SINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_INT;
	case KORE_GPU_TEXTURE_FORMAT_RG16_FLOAT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM_SRGB:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SNORM:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_SNORM;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_INT;
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM_SRGB:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_RGB9E5U_FLOAT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UNORM:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_RG11B10U_FLOAT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RG32_UINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RG32_SINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_INT;
	case KORE_GPU_TEXTURE_FORMAT_RG32_FLOAT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_UINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_SINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_INT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_FLOAT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_UINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_SINT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_INT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_FLOAT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_FLOAT;
	// case KORE_GPU_TEXTURE_FORMAT_STENCIL8:
	//	return 1;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH16_UNORM:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24PLUS_NOTHING8:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24PLUS_STENCIL8:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32FLOAT:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32FLOAT_STENCIL8_NOTHING24:
		return KORE_GPU_TEXTURE_FORMAT_TYPE_FLOAT;
	}

	assert(false);
	return 4;
}

bool kore_gpu_texture_format_is_depth(kore_gpu_texture_format format) {
	switch (format) {
	// case KORE_GPU_TEXTURE_FORMAT_STENCIL8:
	//	return 1;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH16_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24PLUS_NOTHING8:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24PLUS_STENCIL8:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32FLOAT:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32FLOAT_STENCIL8_NOTHING24:
		return true;
	default:
		return false;
	}
	return false;
}
