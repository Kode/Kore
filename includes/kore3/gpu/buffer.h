#ifndef KORE_GPU_BUFFER_HEADER
#define KORE_GPU_BUFFER_HEADER

#include <kore3/global.h>

#include "api.h"

#if defined(KORE_DIRECT3D11)
#include <kore3/direct3d11/buffer_structs.h>
#elif defined(KORE_DIRECT3D12)
#include <kore3/direct3d12/buffer_structs.h>
#elif defined(KORE_METAL)
#include <kore3/metal/buffer_structs.h>
#elif defined(KORE_OPENGL)
#include <kore3/opengl/buffer_structs.h>
#elif defined(KORE_VULKAN)
#include <kore3/vulkan/buffer_structs.h>
#elif defined(KORE_WEBGPU)
#include <kore3/webgpu/buffer_structs.h>
#elif defined(KORE_CONSOLE)
#include <kore3/console/buffer_structs.h>
#else
#error("Unknown GPU backend")
#endif

#include <stdint.h>

/*! \file buffer.h
    \brief Provides various functions for GPU buffers.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_gpu_buffer {
#ifdef KORE_GPU_VALIDATION
	uint32_t usage_flags;
#endif
	KORE_GPU_IMPL(buffer);
} kore_gpu_buffer;

KORE_FUNC void  kore_gpu_buffer_set_name(kore_gpu_buffer *buffer, const char *name);
KORE_FUNC void  kore_gpu_buffer_destroy(kore_gpu_buffer *buffer);
KORE_FUNC void *kore_gpu_buffer_try_to_lock_all(kore_gpu_buffer *buffer);
KORE_FUNC void *kore_gpu_buffer_lock_all(kore_gpu_buffer *buffer);
KORE_FUNC void *kore_gpu_buffer_try_to_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size);
KORE_FUNC void *kore_gpu_buffer_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size);
KORE_FUNC void  kore_gpu_buffer_unlock(kore_gpu_buffer *buffer);

#ifdef __cplusplus
}
#endif

#endif
