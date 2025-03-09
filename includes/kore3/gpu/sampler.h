#ifndef KORE_GPU_SAMPLER_HEADER
#define KORE_GPU_SAMPLER_HEADER

#include <kore3/global.h>

#include "api.h"

#if defined(KORE_DIRECT3D11)
#include <kore3/direct3d11/sampler_structs.h>
#elif defined(KORE_DIRECT3D12)
#include <kore3/direct3d12/sampler_structs.h>
#elif defined(KORE_METAL)
#include <kore3/metal/sampler_structs.h>
#elif defined(KORE_OPENGL)
#include <kore3/opengl/sampler_structs.h>
#elif defined(KORE_VULKAN)
#include <kore3/vulkan/sampler_structs.h>
#elif defined(KORE_WEBGPU)
#include <kore3/webgpu/sampler_structs.h>
#else
#error("Unknown GPU backend")
#endif

/*! \file sampler.h
    \brief Provides functions for handling samplers.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_gpu_sampler {
	KORE_GPU_IMPL(sampler);
} kore_gpu_sampler;

KORE_FUNC void kore_gpu_sampler_set_name(kore_gpu_sampler *sampler, const char *name);

#ifdef __cplusplus
}
#endif

#endif
