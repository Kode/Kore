#include <kore3/gpu/sampler.h>

#ifdef KORE_DIRECT3D12
#include <kope/direct3d12/sampler_functions.h>
#endif

#ifdef KORE_METAL
#include <kope/metal/sampler_functions.h>
#endif

#ifdef KORE_VULKAN
#include <kope/vulkan/sampler_functions.h>
#endif

void kore_gpu_sampler_set_name(kore_gpu_sampler *sampler, const char *name) {
	KORE_GPU_CALL2(sampler_set_name, sampler, name);
}
