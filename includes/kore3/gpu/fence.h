#ifndef KORE_GPU_FENCE_HEADER
#define KORE_GPU_FENCE_HEADER

#include <kore3/global.h>

#include "api.h"

#ifdef KORE_DIRECT3D12
#include <kope/direct3d12/fence_structs.h>
#endif

#ifdef KORE_METAL
#include <kope/metal/fence_structs.h>
#endif

#ifdef KORE_VULKAN
#include <kope/vulkan/fence_structs.h>
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
