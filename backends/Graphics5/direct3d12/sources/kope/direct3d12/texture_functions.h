#ifndef KORE_D3D12_TEXTURE_FUNCTIONS_HEADER
#define KORE_D3D12_TEXTURE_FUNCTIONS_HEADER

#include <kore3/gpu/texture.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t kore_d3d12_texture_resource_state_index(kore_gpu_texture *texture, uint32_t mip_level, uint32_t array_layer);

void kore_d3d12_texture_set_name(kore_gpu_texture *texture, const char *name);

#ifdef __cplusplus
}
#endif

#endif
