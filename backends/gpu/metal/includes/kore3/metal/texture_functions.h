#ifndef KORE_METAL_TEXTURE_FUNCTIONS_HEADER
#define KORE_METAL_TEXTURE_FUNCTIONS_HEADER

#include <kore3/gpu/texture.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_metal_texture_set_name(kore_gpu_texture *texture, const char *name);
void kore_metal_texture_destroy(kore_gpu_texture *texture);

#ifdef __cplusplus
}
#endif

#endif
