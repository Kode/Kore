#ifndef KORE_OPENGL_TEXTURE_STRUCTS_HEADER
#define KORE_OPENGL_TEXTURE_STRUCTS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

// add-ons to kore_gpu_texture_usage, top 16 bits only
typedef enum kore_opengl_texture_usage {
	KORE_OPENGL_TEXTURE_USAGE_SAMPLED     = 0x00010000,
	KORE_OPENGL_TEXTURE_USAGE_STORAGE     = 0x00020000,
	KORE_OPENGL_TEXTURE_USAGE_FRAMEBUFFER = 0x00040000,
} kore_opengl_texture_usage;

typedef struct kore_opengl_texture {
	uint32_t framebuffer;
	uint32_t texture;
	bool     is_primary_framebuffer;

	uint32_t width;
	uint32_t height;
} kore_opengl_texture;

#ifdef __cplusplus
}
#endif

#endif
