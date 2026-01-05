#ifndef KORE_GPU_RAYTRACING_HEADER
#define KORE_GPU_RAYTRACING_HEADER

#include <kore3/global.h>

#include "api.h"

#if defined(KORE_DIRECT3D11)
#include <kore3/direct3d11/raytracing_structs.h>
#elif defined(KORE_DIRECT3D12)
#include <kore3/direct3d12/raytracing_structs.h>
#elif defined(KORE_METAL)
#include <kore3/metal/raytracing_structs.h>
#elif defined(KORE_OPENGL)
#include <kore3/opengl/raytracing_structs.h>
#elif defined(KORE_VULKAN)
#include <kore3/vulkan/raytracing_structs.h>
#elif defined(KORE_WEBGPU)
#include <kore3/webgpu/raytracing_structs.h>
#elif defined(KORE_KOMPJUTA)
#include <kore3/kompjuta/raytracing_structs.h>
#elif defined(KORE_CONSOLE)
#include <kore3/console/raytracing_structs.h>
#else
#error("Unknown GPU backend")
#endif

/*! \file raytracing.h
    \brief Provides raytracing support functions.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_gpu_raytracing_volume {
	KORE_GPU_IMPL(raytracing_volume);
} kore_gpu_raytracing_volume;

typedef struct kore_gpu_raytracing_hierarchy {
	KORE_GPU_IMPL(raytracing_hierarchy);
} kore_gpu_raytracing_hierarchy;

KORE_FUNC void kore_gpu_raytracing_volume_destroy(kore_gpu_raytracing_volume *volume);

KORE_FUNC void kore_gpu_raytracing_hierarchy_destroy(kore_gpu_raytracing_hierarchy *hierarchy);

#ifdef __cplusplus
}
#endif

#endif
