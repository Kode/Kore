#ifndef KORE_GPU_TEXTURE_HEADER
#define KORE_GPU_TEXTURE_HEADER

#include <kore3/global.h>

#include "api.h"
#include "textureformat.h"

#if defined(KORE_DIRECT3D11)
#include <kore3/direct3d11/texture_structs.h>
#elif defined(KORE_DIRECT3D12)
#include <kore3/direct3d12/texture_structs.h>
#elif defined(KORE_METAL)
#include <kore3/metal/texture_structs.h>
#elif defined(KORE_OPENGL)
#include <kore3/opengl/texture_structs.h>
#elif defined(KORE_VULKAN)
#include <kore3/vulkan/texture_structs.h>
#elif defined(KORE_WEBGPU)
#include <kore3/webgpu/texture_structs.h>
#else
#error("Unknown GPU backend")
#endif

/*! \file texture.h
    \brief Provides functions for handling textures.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_gpu_texture {
	uint32_t                width;
	uint32_t                height;
	kore_gpu_texture_format format;

	KORE_GPU_IMPL(texture);
} kore_gpu_texture;

typedef enum kore_gpu_texture_view_dimension {
	KORE_GPU_TEXTURE_VIEW_DIMENSION_1D,
	KORE_GPU_TEXTURE_VIEW_DIMENSION_2D,
	KORE_GPU_TEXTURE_VIEW_DIMENSION_2DARRAY,
	KORE_GPU_TEXTURE_VIEW_DIMENSION_CUBE,
	KORE_GPU_TEXTURE_VIEW_DIMENSION_CUBEARRAY,
	KORE_GPU_TEXTURE_VIEW_DIMENSION_3D
} kore_gpu_texture_view_dimension;

typedef enum kore_gpu_texture_aspect {
	KORE_GPU_IMAGE_COPY_ASPECT_ALL,
	KORE_GPU_IMAGE_COPY_ASPECT_DEPTH_ONLY,
	KORE_GPU_IMAGE_COPY_ASPECT_STENCIL_ONLY
} kore_gpu_texture_aspect;

typedef struct kore_gpu_texture_view {
	kore_gpu_texture               *texture;
	kore_gpu_texture_format         format;
	kore_gpu_texture_view_dimension dimension;
	kore_gpu_texture_aspect         aspect;
	uint32_t                        base_mip_level;
	uint32_t                        mip_level_count;
	uint32_t                        base_array_layer;
	uint32_t                        array_layer_count;
} kore_gpu_texture_view;

KORE_FUNC void kore_gpu_texture_set_name(kore_gpu_texture *texture, const char *name);

KORE_FUNC void kore_gpu_texture_destroy(kore_gpu_texture *texture);

#ifdef __cplusplus
}
#endif

#endif
