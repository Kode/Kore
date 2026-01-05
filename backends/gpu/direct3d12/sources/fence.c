#include <kore3/direct3d12/fence_functions.h>

#include <kore3/gpu/fence.h>

void kore_d3d12_fence_destroy(kore_gpu_fence *fence) {
	COM_CALL_VOID(fence->d3d12.fence, Release);
}
