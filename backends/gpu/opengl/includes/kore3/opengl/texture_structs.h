#ifndef KORE_OPENGL_TEXTURE_STRUCTS_HEADER
#define KORE_OPENGL_TEXTURE_STRUCTS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_opengl_texture {
	uint32_t framebuffer;
	uint32_t texture;
	bool     is_primary_framebuffer;
} kore_opengl_texture;

#ifdef __cplusplus
}
#endif

#endif
