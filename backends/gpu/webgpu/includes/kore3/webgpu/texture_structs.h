#ifndef KORE_WEBGPU_TEXTURE_STRUCTS_HEADER
#define KORE_WEBGPU_TEXTURE_STRUCTS_HEADER

#include "webgpu.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_webgpu_texture {
	WGPUTexture texture;
} kore_webgpu_texture;

#ifdef __cplusplus
}
#endif

#endif
