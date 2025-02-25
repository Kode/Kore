#include "fence_functions.h"

#include <kore3/gpu/fence.h>

#include <kore3/backend/SystemMicrosoft.h>

void kore_d3d12_fence_destroy(kore_gpu_fence *fence) {
	fence->d3d12.fence->Release();
}
