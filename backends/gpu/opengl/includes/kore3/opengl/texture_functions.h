#ifndef KORE_OPENGL_TEXTURE_FUNCTIONS_HEADER
#define KORE_OPENGL_TEXTURE_FUNCTIONS_HEADER

#include <kore3/gpu/texture.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_opengl_texture_set_name(kore_gpu_texture *texture, const char *name);

#ifdef __cplusplus
}
#endif

#endif
