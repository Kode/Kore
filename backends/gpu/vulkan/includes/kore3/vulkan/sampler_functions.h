#ifndef KORE_VULKAN_SAMPLER_FUNCTIONS_HEADER
#define KORE_VULKAN_SAMPLER_FUNCTIONS_HEADER

#include <kore3/gpu/sampler.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_vulkan_sampler_set_name(kore_gpu_sampler *sampler, const char *name);

void kore_vulkan_sampler_destroy(kore_gpu_sampler *sampler);

#ifdef __cplusplus
}
#endif

#endif
