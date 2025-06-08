#ifndef KORE_WEBGPU_TEXTURE_FUNCTIONS_HEADER
#define KORE_WEBGPU_TEXTURE_FUNCTIONS_HEADER

#include <kore3/gpu/texture.h>

#ifdef __cplusplus
extern "C" {
#endif

void kore_webgpu_texture_set_name(kore_gpu_texture *texture, const char *name);
void kore_webgpu_texture_destroy(kore_gpu_texture *texture);

WGPUTextureFormat kore_webgpu_convert_texture_format_to_webgpu(kore_gpu_texture_format format);
kore_gpu_texture_format kore_webgpu_convert_texture_format_to_kore(WGPUTextureFormat format);

#ifdef __cplusplus
}
#endif

#endif
