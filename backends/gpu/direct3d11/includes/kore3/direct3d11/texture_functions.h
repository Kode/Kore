#ifndef KORE_D3D11_TEXTURE_FUNCTIONS_HEADER
#define KORE_D3D11_TEXTURE_FUNCTIONS_HEADER

#include <kore3/gpu/texture.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_d3d11_texture_set_name(kore_gpu_texture *texture, const char *name);

void kore_d3d11_texture_destroy(kore_gpu_texture *texture);

#ifdef __cplusplus
}
#endif

#endif
