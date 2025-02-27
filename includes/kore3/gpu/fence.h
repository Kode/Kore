#ifndef KORE_GPU_FENCE_HEADER
#define KORE_GPU_FENCE_HEADER

#include <kore3/global.h>

#include "api.h"

#if defined(KORE_DIRECT3D11)
#include <kore3/direct3d11/fence_structs.h>
#elif defined(KORE_DIRECT3D12)
#include <kore3/direct3d12/fence_structs.h>
#elif defined(KORE_METAL)
#include <kore3/metal/fence_structs.h>
#elif defined(KORE_OPENGL)
#include <kore3/opengl/fence_structs.h>
#elif defined(KORE_VULKAN)
#include <kore3/vulkan/fence_structs.h>
#elif defined(KORE_WEBGPU)
#include <kore3/webgpu/fence_structs.h>
#else
#error("Unknown GPU backend")
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_gpu_fence {
	KORE_GPU_IMPL(fence);
} kore_gpu_fence;

KORE_FUNC void kore_gpu_fence_destroy(kore_gpu_fence *fence);

#ifdef __cplusplus
}
#endif

#endif
