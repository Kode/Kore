#include <kore3/gpu/sampler.h>

#ifdef KORE_DIRECT3D12
#include <kore3/direct3d12/sampler_functions.h>
#endif

#ifdef KORE_METAL
#include <kore3/metal/sampler_functions.h>
#endif

#ifdef KORE_VULKAN
#include <kore3/vulkan/sampler_functions.h>
#endif

#ifdef KORE_WEBGPU
#include <kore3/webgpu/sampler_functions.h>
#endif

void kore_gpu_sampler_set_name(kore_gpu_sampler *sampler, const char *name) {
	KORE_GPU_CALL2(sampler_set_name, sampler, name);
}
