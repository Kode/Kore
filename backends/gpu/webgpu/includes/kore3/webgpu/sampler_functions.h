#ifndef KORE_WEBGPU_SAMPLER_FUNCTIONS_HEADER
#define KORE_WEBGPU_SAMPLER_FUNCTIONS_HEADER

#include <kore3/gpu/sampler.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_webgpu_sampler_set_name(kore_gpu_sampler *sampler, const char *name);

#ifdef __cplusplus
}
#endif

#endif
