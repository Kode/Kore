#ifndef KORE_D3D12_BUFFER_FUNCTIONS_HEADER
#define KORE_D3D12_BUFFER_FUNCTIONS_HEADER

#include <kore3/gpu/buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_d3d12_buffer_set_name(kore_gpu_buffer *buffer, const char *name);

void kore_d3d12_buffer_destroy(kore_gpu_buffer *buffer);

void *kore_d3d12_buffer_try_to_lock_all(kore_gpu_buffer *buffer);

void *kore_d3d12_buffer_lock_all(kore_gpu_buffer *buffer);

void *kore_d3d12_buffer_try_to_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size);

void *kore_d3d12_buffer_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size);

void kore_d3d12_buffer_unlock(kore_gpu_buffer *buffer);

bool kore_d3d12_buffer_in_use(kore_gpu_buffer *buffer);

void kore_d3d12_buffer_wait_until_not_in_use(kore_gpu_buffer *buffer);

#ifdef __cplusplus
}
#endif

#endif
