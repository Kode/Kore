#include <kore3/gpu/fence.h>

#ifdef KORE_DIRECT3D12
#include <kore3/direct3d12/fence_functions.h>
#endif

void kore_gpu_fence_destroy(kore_gpu_fence *fence) {
	KORE_GPU_CALL1(fence_destroy, fence);
}
