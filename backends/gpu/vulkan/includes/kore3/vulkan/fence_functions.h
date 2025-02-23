#ifndef KORE_VULKAN_FENCE_FUNCTIONS_HEADER
#define KORE_VULKAN_FENCE_FUNCTIONS_HEADER

#include <kore3/gpu/fence.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_vulkan_fence_destroy(kore_gpu_fence *fence);

#ifdef __cplusplus
}
#endif

#endif
