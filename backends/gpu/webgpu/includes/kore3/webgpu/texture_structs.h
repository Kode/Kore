#ifndef KORE_WEBGPU_TEXTURE_STRUCTS_HEADER
#define KORE_WEBGPU_TEXTURE_STRUCTS_HEADER

#include "webgpu.h"

#ifdef __cplusplus
extern "C" {
#endif

// add-ons to kore_gpu_texture_usage, top 16 bits only
typedef enum kore_webgpu_texture_usage {
	KORE_WEBGPU_TEXTURE_USAGE_SAMPLED     = 0x00010000,
	KORE_WEBGPU_TEXTURE_USAGE_STORAGE     = 0x00020000,
	KORE_WEBGPU_TEXTURE_USAGE_FRAMEBUFFER = 0x00040000,
} kore_webgpu_texture_usage;

typedef struct kore_webgpu_texture {
	WGPUTexture             texture;
	kore_gpu_texture_format format;
} kore_webgpu_texture;

#ifdef __cplusplus
}
#endif

#endif
