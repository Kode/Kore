#ifndef KORE_GPU_TEXTUREFORMAT_HEADER
#define KORE_GPU_TEXTUREFORMAT_HEADER

#include <stdbool.h>
#include <stdint.h>

/*! \file textureformat.h
    \brief Provides an enum containing all supported texture formats and some companion functions.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef enum kore_gpu_texture_format {
	KORE_GPU_TEXTURE_FORMAT_R8_UNORM,
	KORE_GPU_TEXTURE_FORMAT_R8_SNORM,
	KORE_GPU_TEXTURE_FORMAT_R8_UINT,
	KORE_GPU_TEXTURE_FORMAT_R8_SINT,
	KORE_GPU_TEXTURE_FORMAT_R16_UINT,
	KORE_GPU_TEXTURE_FORMAT_R16_SINT,
	KORE_GPU_TEXTURE_FORMAT_R16_FLOAT,
	KORE_GPU_TEXTURE_FORMAT_RG8_UNORM,
	KORE_GPU_TEXTURE_FORMAT_RG8_SNORM,
	KORE_GPU_TEXTURE_FORMAT_RG8_UINT,
	KORE_GPU_TEXTURE_FORMAT_RG8_SINT,
	KORE_GPU_TEXTURE_FORMAT_R32_UINT,
	KORE_GPU_TEXTURE_FORMAT_R32_SINT,
	KORE_GPU_TEXTURE_FORMAT_R32_FLOAT,
	KORE_GPU_TEXTURE_FORMAT_RG16_UINT,
	KORE_GPU_TEXTURE_FORMAT_RG16_SINT,
	KORE_GPU_TEXTURE_FORMAT_RG16_FLOAT,
	KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM,
	KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM_SRGB,
	KORE_GPU_TEXTURE_FORMAT_RGBA8_SNORM,
	KORE_GPU_TEXTURE_FORMAT_RGBA8_UINT,
	KORE_GPU_TEXTURE_FORMAT_RGBA8_SINT,
	KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM,
	KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM_SRGB,
	KORE_GPU_TEXTURE_FORMAT_RGB9E5U_FLOAT,
	KORE_GPU_TEXTURE_FORMAT_RGB10A2_UINT,
	KORE_GPU_TEXTURE_FORMAT_RGB10A2_UNORM,
	KORE_GPU_TEXTURE_FORMAT_RG11B10U_FLOAT,
	KORE_GPU_TEXTURE_FORMAT_RG32_UINT,
	KORE_GPU_TEXTURE_FORMAT_RG32_SINT,
	KORE_GPU_TEXTURE_FORMAT_RG32_FLOAT,
	KORE_GPU_TEXTURE_FORMAT_RGBA16_UINT,
	KORE_GPU_TEXTURE_FORMAT_RGBA16_SINT,
	KORE_GPU_TEXTURE_FORMAT_RGBA16_FLOAT,
	KORE_GPU_TEXTURE_FORMAT_RGBA32_UINT,
	KORE_GPU_TEXTURE_FORMAT_RGBA32_SINT,
	KORE_GPU_TEXTURE_FORMAT_RGBA32_FLOAT,
	// KORE_GPU_TEXTURE_FORMAT_STENCIL8, // not available in d3d12
	KORE_GPU_TEXTURE_FORMAT_DEPTH16_UNORM,
	KORE_GPU_TEXTURE_FORMAT_DEPTH24PLUS_NOTHING8,
	KORE_GPU_TEXTURE_FORMAT_DEPTH24PLUS_STENCIL8,
	KORE_GPU_TEXTURE_FORMAT_DEPTH32FLOAT,
	KORE_GPU_TEXTURE_FORMAT_DEPTH32FLOAT_STENCIL8_NOTHING24,
	// TODO: compressed formats
} kore_gpu_texture_format;

uint32_t kore_gpu_texture_format_byte_size(kore_gpu_texture_format format);

bool kore_gpu_texture_format_is_depth(kore_gpu_texture_format format);

#ifdef __cplusplus
}
#endif

#endif
