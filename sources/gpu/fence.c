#include <kore3/gpu/fence.h>

#if defined(KORE_DIRECT3D11)
#include <kore3/direct3d11/fence_functions.h>
#elif defined(KORE_DIRECT3D12)
#include <kore3/direct3d12/fence_functions.h>
#elif defined(KORE_METAL)
#include <kore3/metal/fence_functions.h>
#elif defined(KORE_OPENGL)
#include <kore3/opengl/fence_functions.h>
#elif defined(KORE_VULKAN)
#include <kore3/vulkan/fence_functions.h>
#elif defined(KORE_WEBGPU)
#include <kore3/webgpu/fence_functions.h>
#else
#error("Unknown GPU backend")
#endif

void kore_gpu_fence_destroy(kore_gpu_fence *fence) {
	KORE_GPU_CALL1(fence_destroy, fence);
}
