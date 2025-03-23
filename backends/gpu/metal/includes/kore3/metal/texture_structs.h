#ifndef KORE_METAL_TEXTURE_STRUCTS_HEADER
#define KORE_METAL_TEXTURE_STRUCTS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_metal_texture {
	void   *texture;
	uint8_t dimension;
} kore_metal_texture;

#ifdef __cplusplus
}
#endif

#endif
