#ifndef KORE_D3D12_SAMPLER_FUNCTIONS_HEADER
#define KORE_D3D12_SAMPLER_FUNCTIONS_HEADER

#include <kore3/gpu/sampler.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_d3d12_sampler_set_name(kore_gpu_sampler *sampler, const char *name);

#ifdef __cplusplus
}
#endif

#endif
