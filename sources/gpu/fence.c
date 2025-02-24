#include <kore3/gpu/fence.h>

#ifdef KORE_DIRECT3D12
#include <kore3/direct3d12/fence_functions.h>
#endif

#ifdef KORE_METAL
#include <kore3/metal/fence_functions.h>
#endif

#ifdef KORE_VULKAN
#include <kore3/vulkan/fence_functions.h>
#endif

#ifdef KORE_WEBGPU
#include <kore3/webgpu/fence_functions.h>
#endif

void kore_gpu_fence_destroy(kore_gpu_fence *fence) {
	KORE_GPU_CALL1(fence_destroy, fence);
}
