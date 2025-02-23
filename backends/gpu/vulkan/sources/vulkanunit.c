#include "vulkanunit.h"

#include <kore3/gpu/device.h>

#include <assert.h>

static VkFormat convert_format(kore_gpu_texture_format format) {
	switch (format) {
	case KORE_GPU_TEXTURE_FORMAT_R8_UNORM:
		return VK_FORMAT_R8_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_R8_SNORM:
		return VK_FORMAT_R8_SNORM;
	case KORE_GPU_TEXTURE_FORMAT_R8_UINT:
		return VK_FORMAT_R8_UINT;
	case KORE_GPU_TEXTURE_FORMAT_R8_SINT:
		return VK_FORMAT_R8_SINT;
	case KORE_GPU_TEXTURE_FORMAT_R16_UINT:
		return VK_FORMAT_R16_UINT;
	case KORE_GPU_TEXTURE_FORMAT_R16_SINT:
		return VK_FORMAT_R16_SINT;
	case KORE_GPU_TEXTURE_FORMAT_R16_FLOAT:
		return VK_FORMAT_R16_SFLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RG8_UNORM:
		return VK_FORMAT_R8G8_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_RG8_SNORM:
		return VK_FORMAT_R8G8_SNORM;
	case KORE_GPU_TEXTURE_FORMAT_RG8_UINT:
		return VK_FORMAT_R8G8_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RG8_SINT:
		return VK_FORMAT_R8G8_SINT;
	case KORE_GPU_TEXTURE_FORMAT_R32_UINT:
		return VK_FORMAT_R32_UINT;
	case KORE_GPU_TEXTURE_FORMAT_R32_SINT:
		return VK_FORMAT_R32_SINT;
	case KORE_GPU_TEXTURE_FORMAT_R32_FLOAT:
		return VK_FORMAT_R32_SFLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RG16_UINT:
		return VK_FORMAT_R16G16_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RG16_SINT:
		return VK_FORMAT_R16G16_SINT;
	case KORE_GPU_TEXTURE_FORMAT_RG16_FLOAT:
		return VK_FORMAT_R16G16_SFLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM:
		return VK_FORMAT_R8G8B8A8_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM_SRGB:
		return VK_FORMAT_R8G8B8A8_SRGB;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SNORM:
		return VK_FORMAT_R8G8B8A8_SNORM;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UINT:
		return VK_FORMAT_R8G8B8A8_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SINT:
		return VK_FORMAT_R8G8B8A8_SINT;
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM:
		return VK_FORMAT_B8G8R8A8_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM_SRGB:
		return VK_FORMAT_B8G8R8A8_SRGB;
	case KORE_GPU_TEXTURE_FORMAT_RGB9E5U_FLOAT:
		return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UINT:
		return VK_FORMAT_A2R10G10B10_UINT_PACK32;
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UNORM:
		return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
	case KORE_GPU_TEXTURE_FORMAT_RG11B10U_FLOAT:
		return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
	case KORE_GPU_TEXTURE_FORMAT_RG32_UINT:
		return VK_FORMAT_R32G32_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RG32_SINT:
		return VK_FORMAT_R32G32_SINT;
	case KORE_GPU_TEXTURE_FORMAT_RG32_FLOAT:
		return VK_FORMAT_R32G32_SFLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_UINT:
		return VK_FORMAT_R16G16B16A16_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_SINT:
		return VK_FORMAT_R16G16B16A16_SINT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_FLOAT:
		VK_FORMAT_R16G16B16A16_SFLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_UINT:
		return VK_FORMAT_R32G32B32A32_UINT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_SINT:
		return VK_FORMAT_R32G32B32A32_SINT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_FLOAT:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH16_UNORM:
		return VK_FORMAT_D16_UNORM;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24PLUS_NOTHING8:
		return VK_FORMAT_X8_D24_UNORM_PACK32;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24PLUS_STENCIL8:
		return VK_FORMAT_D24_UNORM_S8_UINT;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32FLOAT:
		return VK_FORMAT_D32_SFLOAT;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32FLOAT_STENCIL8_NOTHING24:
		return VK_FORMAT_D32_SFLOAT_S8_UINT;
	}

	return VK_FORMAT_R8G8B8A8_UNORM;
}

#include "buffer.c"
#include "commandlist.c"
#include "descriptorset.c"
#include "device.c"
#include "pipeline.c"
#include "sampler.c"
#include "texture.c"
