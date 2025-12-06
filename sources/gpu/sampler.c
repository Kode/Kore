#include <kore3/gpu/sampler.h>

#if defined(KORE_DIRECT3D11)
#include <kore3/direct3d11/sampler_functions.h>
#elif defined(KORE_DIRECT3D12)
#include <kore3/direct3d12/sampler_functions.h>
#elif defined(KORE_METAL)
#include <kore3/metal/sampler_functions.h>
#elif defined(KORE_OPENGL)
#include <kore3/opengl/sampler_functions.h>
#elif defined(KORE_VULKAN)
#include <kore3/vulkan/sampler_functions.h>
#elif defined(KORE_WEBGPU)
#include <kore3/webgpu/sampler_functions.h>
#elif defined(KORE_CONSOLE)
#include <kore3/console/sampler_functions.h>
#else
#error("Unknown GPU backend")
#endif

void kore_gpu_sampler_set_name(kore_gpu_sampler *sampler, const char *name) {
	KORE_GPU_CALL2(sampler_set_name, sampler, name);
}

void kore_gpu_sampler_destroy(kore_gpu_sampler *sampler) {
	KORE_GPU_CALL1(sampler_destroy, sampler);
}
