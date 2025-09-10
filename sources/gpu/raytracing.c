#include <kore3/gpu/raytracing.h>

#if defined(KORE_DIRECT3D11)
#include <kore3/direct3d11/raytracing_functions.h>
#elif defined(KORE_DIRECT3D12)
#include <kore3/direct3d12/raytracing_functions.h>
#elif defined(KORE_METAL)
#include <kore3/metal/raytracing_functions.h>
#elif defined(KORE_OPENGL)
#include <kore3/opengl/raytracing_functions.h>
#elif defined(KORE_VULKAN)
#include <kore3/vulkan/raytracing_functions.h>
#elif defined(KORE_WEBGPU)
#include <kore3/webgpu/raytracing_functions.h>
#else
#error("Unknown GPU backend")
#endif

void kore_gpu_raytracing_volume_destroy(kore_gpu_raytracing_volume *volume) {
	KORE_GPU_CALL1(raytracing_volume_destroy, volume);
}

void kore_gpu_raytracing_hierarchy_destroy(kore_gpu_raytracing_hierarchy *hierarchy) {
	KORE_GPU_CALL1(raytracing_hierarchy_destroy, hierarchy);
}
